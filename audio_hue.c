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
#include <curl/curl.h>
#include <fftw3.h>
#include <iso646.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>



typedef struct audio_s {
    float value;
    float decay;
    float threshold;
    float prev;
    int dead;
} audio_t;

typedef struct lamp_s {
    int deadMax;
    int deadCounter;
} lamp_t;


static const int N = 1024;
static const int HUE_DEAD = 25;

static char *hueIP = NULL;
static char *hueID = NULL;
static int hueLampCount = 0;
static int *hueLampMap = NULL;




static CURL **curlHue;
static char curlRequestBuffer[1024];

static signed short *buffer;
static unsigned int bufferFill = 0;

static fftwf_plan p;
static float *in = NULL;
static fftwf_complex *out = NULL;
static float *outL = NULL;
static float *outR = NULL;

static audio_t audio;
static lamp_t *lamp;


static size_t curlWriteFunction(char *ptr, size_t size, size_t nmemb, void *userdata) {
    //	printf( "%lu %lu\n", size, nmemb );
    //	puts( ptr );
    return size * nmemb;
}


static CURL *curlSetup(const int in_I) {
    char uri[1024];
    sprintf(uri, "http://%s/api/%s/lights/%d/state", hueIP, hueID, in_I);

    CURL *curl = curl_easy_init();
    assert(curl);

    //curl_easy_setopt( curl, CURLOPT_VERBOSE, 1 );
    curl_easy_setopt(curl, CURLOPT_URL, uri);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFunction);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, curlRequestBuffer);

    //fprintf( stderr, "Save light settings for %s!\n", uri );

    return curl;
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
            outL[i] = cabsf(out[i]) / (float)(N / 2 );
        }
    }

    {
        // right channel
        for (i = 0; i < N; i++) {
            in[i] = buffer[i * 2 + 1];
        }

        fftwf_execute(p);

        for (i = 0; i < N / 2 + 1; i++) {
            outR[i] = cabsf(out[i]) / (float)(N / 2);
        }
    }

    // living White
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

        float steepness = sigma - audio.prev;

        if ((steepness > 0.05) and (sigma - audio.value > 0) and (audio.dead < 0)) {
            audio.value = sigma;
            audio.decay = audio.value * 0.025f;
            audio.dead = 5;

            int lampID = -1;
            int lampIDDeadness = 1024;

            for (i = 0; i < hueLampCount; i++) {
                if (lamp[i].deadCounter <= lampIDDeadness) {
                    lampID = i;
                    lampIDDeadness = lamp[i].deadCounter;
                }
            }

            if ((lampID > -1) and (lamp[lampID].deadCounter < 0)) {
                int saturatedSteepness = 1024 * steepness;
                saturatedSteepness = (saturatedSteepness > 255) ? 255 : saturatedSteepness;
                lamp[lampID].deadCounter = 0; //HUE_DEAD + rand() / (RAND_MAX / HUE_DEAD);
                lamp[lampID].deadMax = lamp[lampID].deadCounter;
                //fprintf( stderr, "%i (%i)", saturatedSteepness, lamp[lampID].deadCounter );
                sprintf(curlRequestBuffer, "{ \"bri\":%i, \"transitiontime\":0 }", saturatedSteepness);
                curl_easy_perform(curlHue[lampID]);
            }

            //fprintf( stderr, "%f ", steepness );
            //fputs( "Beep\n", stderr );
        } else {
            audio.dead--;
            audio.value -= audio.decay;

            if (audio.value < 0) {
                audio.value = 0;
            }

            //fputs( ".", stderr );
        }

        audio.prev = sigma;
    }

    for (i = 0; i < hueLampCount; i++) {
        if (lamp[i].deadCounter == lamp[i].deadMax - 5) {   // wait 5 tics until reset
            //fprintf( stderr, "%i", i );
            sprintf(curlRequestBuffer, "{ \"bri\":0, \"transitiontime\":5 }");
            curl_easy_perform(curlHue[i]);
        }

        lamp[i].deadCounter--;
    }
}





int Fs;
long long starttime, samples_played;


static void help(void) {
    puts("    -b ip               the IP address of hue bridge");
    puts("    -i id               the identifier used to access the hue bridge");
    puts("    -l lamps            a comma separated list of the number of hue lamps");
    puts("                        -l 2,3,5 will only light the lamps matching the given IDs");
}


static int init(int argc, char **argv) {
    // set up default values first
    config.audio_backend_buffer_desired_length = 1.0;
    config.audio_backend_latency_offset = (float)(-2*N)/44100;

    // get settings from settings file
    // do the "general" audio  options. Note, these options are in the "general" stanza!
    parse_general_audio_options();


    optind = 1; // optind=0 is equivalent to optind=1 plus special behaviour
    argv--;     // so we shift the arguments to satisfy getopt()
    argc++;

    // some platforms apparently require optreset = 1; - which?
    int opt;

    while ((opt = getopt(argc, argv, "b:i:l:")) > 0) {
        switch (opt) {
            case 'b':
                hueIP = optarg;
                break;

            case 'i':
                hueID = optarg;
                break;

            case 'l': {
                    const char *sep = ",";
                    char *token;
                    char copyOf[1024];
                    strcpy(copyOf, optarg);

                    for (token = strtok(copyOf, sep); token; token = strtok(NULL, sep)) {
                        hueLampCount++;
                    }

                    hueLampMap = malloc(sizeof(int) * hueLampCount);
                    int hueLampMapFill = 0;
                    strcpy(copyOf, optarg);

                    for (token = strtok(copyOf, sep); token; token = strtok(NULL, sep)) {
                        hueLampMap[hueLampMapFill] = atoi(token);
                        hueLampMapFill++;
                        //puts( token );
                    }

                    break;
                }

            default:
                help();
                die("Invalid audio option -%c specified", opt);
        }
    }

    if (optind < argc) {
        die("Invalid audio argument: %s", argv[optind]);
    }

    if (!hueIP) {
        die("ip to hue bridge missing!");
    }

    if (!hueID) {
        die("access identifier to hue bridge missing!");
    }

    if (!hueLampCount) {
        die("hue lamp count missing!");
    }

    int i = 0;

    buffer = malloc(sizeof(short) * N * 2);
    curlHue = malloc(sizeof(CURL *) * hueLampCount);
    lamp = malloc(sizeof(lamp_t) * hueLampCount);

    // FFT
    in = (float *) fftwf_malloc(sizeof(float) * N);
    out = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * N);
    outL = (float *) malloc(sizeof(float) * (N / 2 + 1));
    outR = (float *) malloc(sizeof(float) * (N / 2 + 1));

    fftwf_import_system_wisdom();
    p = fftwf_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);   //FFTW_MEASURE
    // fputs( fftwf_export_wisdom_to_string(), stderr );

    // HUE
    audio.value = 0;
    audio.decay = 0;
    audio.threshold = 0.05f;
    audio.prev = 0;
    audio.dead = 0;

    for (i = 0; i < hueLampCount; i++) {
        lamp[i].deadMax = 0;
        lamp[i].deadCounter = 0;
    }

    // CURL
    //CURLcode curlCode = curl_global_init( CURL_GLOBAL_NOTHING );
    curl_global_init(CURL_GLOBAL_NOTHING);

    for (i = 0; i < hueLampCount; i++) {
        curlHue[i] = curlSetup(hueLampMap[i]);
        assert(curlHue[i]);
    }

    return 0;
}


static void deinit(void) {
    fftwf_destroy_plan(p);
    fftwf_free(in);
    fftwf_free(out);
    free(outL);
    free(outR);
    free(buffer);
    free(lamp);
    free(curlHue);
    free(hueLampMap);
}


static void start(int sample_rate, int sample_format) {
    Fs = sample_rate;
    starttime = 0;
    samples_played = 0;
    printf("hue output started at Fs=%d Hz\n", sample_rate);

    int i;

    for (i = 0; i < hueLampCount; i++) {
        sprintf(curlRequestBuffer, "{ \"on\":true, \"bri\":0, \"sat\":255, \"hue\":%i, \"transitiontime\":1, \"effect\":\"colorloop\" }", 65535 * i / hueLampCount);
        //fputs( curlRequestBuffer, stderr );
        curl_easy_perform(curlHue[i]);
    }
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
    int i = 0;

    //fprintf( stderr, "Restore light settings!\n" );
    sprintf(curlRequestBuffer, "{ \"on\":true, \"bri\":0, \"ct\":467, \"effect\":\"none\" }");

    for (i = 0; i < hueLampCount; i++) {
        curl_easy_perform(curlHue[i]);
    }

    // repeat because some update broke the functionality to stop an effect and set something else simultaneously
    for (i = 0; i < hueLampCount; i++) {
        curl_easy_perform(curlHue[i]);
    }

    printf("hue stopped\n");
}


static int delay(long *the_delay) {
    *the_delay = 2 * N;
    return 0;
}


audio_output audio_hue = {
    .name = "hue",
    .help = &help,
    .init = &init,
    .deinit = &deinit,
    .start = &start,
    .stop = &stop,
    .flush = &flush,
    .delay = &delay,
    .play = &play,
    .volume = NULL,
    .parameters = NULL,
    .mute = NULL
};
