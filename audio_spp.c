/*
 * dummy output driver. This file is part of Shairport.
 * Copyright (c) James Laird 2013
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "audio.h"

#include "common.h"

#include <assert.h>
#include <complex.h>
#include <fcntl.h>
#include <fftw3.h>
#include <iso646.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>

#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>



static const int N = 1024;

static signed short *buffer;
static unsigned int bufferFill = 0;

static fftwf_plan p;
static float *in = NULL;
static fftwf_complex *out = NULL;
static float *outL = NULL;
static float *outR = NULL;


static const float MAX_R = 255.0f;
static const float MAX_G = 63.0f;
static const float MAX_B = 23.0f;

static char* s_address = NULL;
static int s_socket = 0;
static bool s_idle = true;


static void setIdle(const bool in_IDLE) {
    s_idle = in_IDLE;
}



static void commitSPP(const uint8_t in_R, const uint8_t in_G, const uint8_t in_B) {
    // state of the light strip
    static uint8_t rgbw[10];
    static uint8_t prev[10];
    sprintf(rgbw, "#%02x%02x%02x%02x", in_R, in_G, in_B, 0);

    int cmp = strncmp(prev, rgbw, 10);

    if (cmp != 0) {
        memcpy(prev, rgbw, 10);

        ssize_t numBytes = write(s_socket, rgbw, 10);
        printf("%s\n", rgbw);
        assert(numBytes == 10);
    }
}



static inline float clampf(const float in_X, const float in_MIN, const float in_MAX) {
    return fminf(fmaxf(in_X, in_MIN), in_MAX);
}



typedef struct Component_s {
    float prev;
    float value;
    float decay;
    int dead;
} Component_t;



static Component_t s_main;
static Component_t components[3];


static void commit(const float in_VALUE) {
    int c;
    //printf( "%f\n", in_VALUE );

    float diff = in_VALUE - s_main.prev;

    if ((diff > 0.05) and (in_VALUE - s_main.value > 0) and (s_main.dead == 0)) {
        s_main.dead = 5;
        s_main.value = in_VALUE;
        s_main.decay = in_VALUE * 0.025f;

        //puts("x");

        int index = -1;
        int minDeadness = INT_MAX;

        for (c = 0; c < 3; c++) {
            if (components[c].dead < minDeadness) {
                index = c;
                minDeadness = components[c].dead;
            }
        }

        if (index != -1) {
            c = index;
            components[c].dead = 100 * diff;
            components[c].value = 4 * diff;
            components[c].decay = components[c].value * 0.025f;
        }
    }



    if (s_main.dead > 0) {
        s_main.dead--;
    }

    s_main.value -= s_main.decay;

    if (s_main.value < 0) {
        s_main.value = 0;
        s_main.decay = 0;
    }

    s_main.prev = in_VALUE;

    //fputs(".", stdout);
}


static void draw() {
    static uint16_t idleCounter = 0;

    if (s_idle) {
        float idleCounterf = (float)(idleCounter);
        float dr1 = 3.0f - fabsf((idleCounterf * 1.5f -    0.0f) / 256.0f);
        float dr2 = 3.0f - fabsf((idleCounterf * 1.5f - 3072.0f) / 256.0f);
        float dr = fmaxf(dr1, dr2);
        float dg = 3.0f - fabsf((idleCounterf * 1.5f - 1024.0f) / 256.0f);
        float db = 3.0f - fabsf((idleCounterf * 1.5f - 2048.0f) / 256.0f);

        dr = clampf(dr * MAX_R * 0.25, 1.0f, MAX_R);
        dg = clampf(dg * MAX_G * 0.25, 1.0f, MAX_G);
        db = clampf(db * MAX_B * 0.25, 1.0f, MAX_B);

        uint8_t rr = dr;
        uint8_t gg = dg;
        uint8_t bb = db;

        commitSPP(rr, gg, bb);

        idleCounter++;

        if (idleCounter == 256 * 8) {
            idleCounter = 0;
        }
    } else {
        int c;
        float dr = clampf(components[0].value * MAX_R, 1.0f, MAX_R);
        float dg = clampf(components[1].value * MAX_R, 1.0f, MAX_R);
        float db = clampf(components[2].value * MAX_R, 1.0f, MAX_R);

        uint8_t rr = dr;
        uint8_t gg = dg;
        uint8_t bb = db;

        commitSPP(rr, gg, bb);


        for (c = 0; c < 3; c++) {
            components[c].dead--;
            components[c].value -= components[c].decay;

            if (components[c].value < 0) {
                components[c].value = 0;
                components[c].decay = 0;
            }
        }
    }
}


static pthread_t s_renderingThread;
static bool s_renderingThreadAlive = true;


static void *sppThread(void *argument) {
    while (s_renderingThreadAlive) {
        draw();
        usleep(40000);   // 25Hz
    }

    return NULL;
}


static void doit() {
    int i;

    {
        // left channel
        for (i = 0; i < N; i++) {
            in[i] = buffer[i * 2 + 0];
        }

        fftwf_execute(p);

        for (i = 0; i < N / 2 + 1; i++) {
            outL[i] = cabsf(out[i]) / (float)N;
        }
    }

    {
        // right channel
        for (i = 0; i < N; i++) {
            in[i] = buffer[i * 2 + 1];
        }

        fftwf_execute(p);

        for (i = 0; i < N / 2 + 1; i++) {
            outR[i] = cabsf(out[i]) / (float)N;
        }
    }

    // SPP
    {
        float mu = 0;

        for (i = 0; i < N / 4; i++) {
            mu += outL[i] + outR[i];
        }

        mu /= N / 4;
        mu /= N;

        float sigma = 0;

        for (i = 0; i < N / 4; i++) {
            sigma += (outL[i] + outR[i] - mu) * (outL[i] + outR[i] - mu);
        }

        sigma /= N / 4;
        sigma = sqrtf(sigma);
        sigma /= N;

        commit(sigma);
    }
}



int Fs;
uint64_t starttime, samples_played;



static void help(void) {
    puts("    -d address           address of the paired bluetooth spp device like \"01:23:45:67:89:ab\"");
}



static int init(int argc, char **argv) {
    optind = 1; // optind=0 is equivalent to optind=1 plus special behaviour
    argv--;     // so we shift the arguments to satisfy getopt()
    argc++;

    // some platforms apparently require optreset = 1; - which?
    int opt;

    while ((opt = getopt(argc, argv, "d:a:")) > 0) {
        switch (opt) {
            case 'd':
                s_address = optarg;
                break;

            default:
                help();
                die("Invalid audio option -%c specified", opt);
        }
    }

    if (optind < argc) {
        die("Invalid audio argument: %s", argv[optind]);
    }

    if (!s_address) {
        die("bluetooth spp device address missing!");
    }

    buffer = malloc(sizeof(short) * N * 2);


    // FFT
    static const char wisdomString[] = "(fftw-3.3.4 fftwf_wisdom #xca4daf64 #xc8f59ea6 #x586875c9 #x14018994"
    "  (fftwf_codelet_r2cf_32 0 #x1040 #x1040 #x0 #xf0a3d344 #x13d3ea67 #x6c559355 #xb97dd65d)"
    "  (fftwf_codelet_hc2cf_32 0 #x1040 #x1040 #x0 #xe9ef8750 #xcfc97096 #xf9e7e48d #x6e5a4034)"
    "  (fftwf_codelet_r2cfII_32 2 #x1040 #x1040 #x0 #x328c26e0 #xd5defb3b #x3f890bcb #xae29c390)"
    "  (fftwf_rdft_vrank_geq1_register 1 #x1040 #x1040 #x0 #x24270b46 #x2c4e5fb2 #x7c394654 #x3261a5dd)"
    "  (fftwf_codelet_r2cf_32 2 #x1040 #x1040 #x0 #xbfa7b557 #xfc0285f7 #xc081451b #xd3d93d06)"
    ")";

    in = (float *) fftwf_malloc(sizeof(float) * N);
    out = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * N);
    outL = (float *) malloc(sizeof(float) * (N / 2 + 1));
    outR = (float *) malloc(sizeof(float) * (N / 2 + 1));

    fftwf_import_wisdom_from_string(wisdomString);
    p = fftwf_plan_dft_r2c_1d(N, in, out, FFTW_EXHAUSTIVE);   //FFTW_MEASURE
    //fputs( fftwf_export_wisdom_to_string(), stderr );


    // SPP
    s_socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    assert(s_socket >= 0);

    struct sockaddr_rc addr = { 0 };
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba(s_address, &addr.rc_bdaddr);

    int status = connect(s_socket, (struct sockaddr *)&addr, sizeof(addr));
    assert(status >= 0);

    commitSPP(1, 1, 1);

    int rc = pthread_create(&s_renderingThread, NULL, sppThread, NULL);
    assert(0 == rc);

    return 0;
}



static void deinit(void) {
    fftwf_destroy_plan(p);
    fftwf_free(in);
    fftwf_free(out);
    free(outL);
    free(outR);
    free(buffer);

    s_renderingThreadAlive = false;
    pthread_join(s_renderingThread, NULL);
    commitSPP(0, 0, 0);

    int err = close(s_socket);
    assert(err >= 0);
}



static void start(int sample_rate) {
    Fs = sample_rate;
    starttime = 0;
    samples_played = 0;
    printf("SPP output started at Fs=%d Hz\n", sample_rate);
    setIdle(false);
}



static void flush(void) {
    bufferFill = 0;
    starttime = 0;
    samples_played = 0;
}



static void play(short buf[], int samples) {
    struct timeval tv;
    uint64_t nowtime;
    int i = 0;
    gettimeofday(&tv, NULL);

    if (!starttime) {
        nowtime = tv.tv_usec + 1000000 * tv.tv_sec;
        starttime = nowtime;
    }

    while (i < samples) {
        buffer[bufferFill] = buf[i];
        bufferFill++;

        if (bufferFill == N * 2) {
            doit();
            bufferFill = 0;

            samples_played += N * 2;
            uint64_t finishtime = starttime + samples_played * 1000000 / Fs;
            nowtime = tv.tv_usec + 1000000 * tv.tv_sec;
            int sleepDuration = (int)(finishtime - nowtime);

            if (sleepDuration > 0) {
                usleep(sleepDuration);
            }
        }

        i++;
    }
}



static void stop(void) {
    setIdle(true);
    printf("SPP stopped\n");
}



audio_output audio_spp = {
    .name = "spp",
    .help = &help,
    .init = &init,
    .deinit = &deinit,
    .start = &start,
    .stop = &stop,
    .flush = &flush,
    .delay = NULL,
    .play = &play,
    .volume = NULL,
    .parameters = NULL,
    .mute = NULL
};
