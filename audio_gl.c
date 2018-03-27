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
#include "config.h"

#include "GLESVisualizer/globaltime.h"
#include "GLESVisualizer/eglcore.h"
#include "GLESVisualizer/Arabesque.h"
#include "GLESVisualizer/Math3D.h"

#include <assert.h>
#include <complex.h>
#include <fftw3.h>
#include <sys/time.h>
#include <iso646.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <bcm_host.h>


static const int N = 1024;

static signed short *buffer;
static unsigned int bufferFill = 0;

static fftwf_plan p;
static float *in = NULL;
static fftwf_complex *out = NULL;
static float *outL = NULL;
static float *outR = NULL;



static const uint16_t s_DISPLAY_NUMBER = 0; // LCD = 0
static pthread_t s_renderingThread;
static bool s_renderingThreadAlive = true;
static uint32_t s_screenWidth = 0;
static uint32_t s_screenHeight = 0;
static uint32_t s_screenFrameRate = 1;


static pthread_t s_metaDataThread;


static void *glThread(void *argument) {
    float s_timeStamp = 0;
    uint32_t frameCounter = 0;

    //initEGL( s_DISPLAY_NUMBER, s_screenWidth/2, s_screenHeight/2, s_screenWidth/2, s_screenHeight/2 );
    initEGL(s_DISPLAY_NUMBER, 0, 0, s_screenWidth, s_screenHeight);

    while (s_renderingThreadAlive) {
        drawEGL();

        frameCounter++;

        if (frameCounter == s_screenFrameRate * 2) {
            float now = timeGet();
            float deltaT = now - s_timeStamp;
            //printf( "%.1f FPS\n", ( s_screenFrameRate * 2.0f ) / deltaT );
            //fputs( ".", stdout );
            fflush(stdout);
            frameCounter = 0;
            s_timeStamp = now;
        }
    }

    destroyEGL();
    return NULL;
}


static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'
                               };

static int decoding_table[256]; // an incoming char can range over ASCII, but by mistake could be all 8 bits.


void initialise_decoding_table() {
    int i;

    for (i = 0; i < 64; i++) {
        decoding_table[(unsigned char) encoding_table[i]] = i;
    }
}


int base64_decode(const char *data,
                  size_t input_length,
                  unsigned char *decoded_data,
                  size_t *output_length) {

    //remember somewhere to call initialise_decoding_table();

    if (input_length % 4 != 0) {
        return -1;
    }

    size_t calculated_output_length = input_length / 4 * 3;

    if (data[input_length - 1] == '=') {
        calculated_output_length--;
    }

    if (data[input_length - 2] == '=') {
        calculated_output_length--;
    }

    if (calculated_output_length > *output_length) {
        return (-1);
    }

    *output_length = calculated_output_length;

    int i, j;

    for (i = 0, j = 0; i < input_length;) {
        if (data[i] >= 123) {
            putc(data[i], stdout);
            fflush(stdout);
        }

        assert(data[i] < 123);
        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6)
                          + (sextet_b << 2 * 6)
                          + (sextet_c << 1 * 6)
                          + (sextet_d << 0 * 6);

        if (j < *output_length) {
            decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        }

        if (j < *output_length) {
            decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        }

        if (j < *output_length) {
            decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
        }
    }

    return 0;
}


static const size_t ARTWORK_SIZE_MAX = 1048576;
static char s_metaTitle[256] = { '\0' };
static char s_metaArtist[256] = { '\0' };
static char s_metaAlbum[256] = { '\0' };
static char *s_metaArtwork = NULL;
static size_t s_metaArtworkSize = 0;


static void *metaDataThread(void *argument) {
    int fd;
    FILE *fs;
    fd_set rfds;
    int retval;

    initialise_decoding_table();

    fs = fopen(config.metadata_pipename, "rb");
    assert(fs);
    fd = fileno(fs);

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    while (s_renderingThreadAlive) {
        int tag_found = 0;
        char str[1024];

        while (tag_found == 0) {
            retval = select(fd + 1, &rfds, NULL, NULL, NULL);

            if (retval == -1) {
                perror("select()");
            } else if (retval) {
                char *rp;
                rp = fgets(str, 1024, fs);

                if (rp != NULL) {
                    if (str[0] == '<') {
                        tag_found = 1;
                    }
                }
            } else {
                // No data
            }
        }

        uint32_t type, code, length;
        char tagend[1024];
        int ret = sscanf(str, "<item><type>%8x</type><code>%8x</code><length>%u</length>", &type, &code, &length);

        if (ret == 3) {
            size_t outputlength = 0;
            char *payload = NULL;

            if (length > 0) {
                payload = malloc(length + 1);

                if (payload) {
                    char datatagstart[64], datatagend[64];
                    memset(datatagstart, 0, 64);
                    int rc = fscanf(fs, "<data encoding=\"base64\">");

                    if (rc == 0) {
                        // now, read in that big (possibly) base64 buffer
                        int c = fgetc(fs);
                        uint32_t b64size = 4 * ((length + 2) / 3);
                        char *b64buf = malloc(b64size + 1);
                        memset(b64buf, 0, b64size + 1);

                        if (fgets(b64buf, b64size + 1, fs) != NULL) {
                            outputlength = length;

                            if (base64_decode(b64buf, b64size, payload, &outputlength) != 0) {
                                length = 0;
                                outputlength = 0;
                                printf("Failed to decode it.\n");
                            }

                            free(b64buf);
                        } else {
                            // couldn't allocate memory for base64 stuff
                        }

                        rc = fscanf(fs, "%64s", datatagend);

                        if (strcmp(datatagend, "</data></item>") != 0) {
                            printf("End data tag not seen, \"%s\" seen instead.\n", datatagend);
                        }
                    }
                } else {
                    // couldn't allocate memory for decoded base64 stuff
                }
            }

            // printf("Got it decoded. Length of decoded string is %u bytes.\n",outputlength);
            if (payload) {
                payload[outputlength] = 0;
            }

            // this has more information about tags, which might be relevant:
            // https://code.google.com/p/ytrack/wiki/DMAP
            switch (code) {
                case 'asal':
                    strncpy(s_metaAlbum, payload, 255);
                    break;

                case 'asar':
                    strncpy(s_metaArtist, payload, 255);
                    break;

                case 'minm':
                    strncpy(s_metaTitle, payload, 255);
                    break;

                case 'PICT':
                    if (outputlength <= ARTWORK_SIZE_MAX) {
                        memcpy(s_metaArtwork, payload, outputlength);
                        s_metaArtworkSize = outputlength;
                    } else {
                        memset(s_metaArtwork, 0, ARTWORK_SIZE_MAX);
                        s_metaArtworkSize = 0;
                    }
                    
                    break;

                default:
                    break;
            }

            if (payload) {
                free(payload);
            }
        }

        // flush stdout, to be able to pipe it later
        fflush(stdout);
    }

    fclose(fs);
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
            outL[i] = cabsf(out[i]) / (float)(N * SHRT_MAX);
            outL[i] = 2 * cbrt(outL[i]);
            outL[i] = smoothstepf(0, 1, outL[i] * 1.5f - 0.5f);
        }
    }

    {
        // right channel
        for (i = 0; i < N; i++) {
            in[i] = buffer[i * 2 + 1];
        }

        fftwf_execute(p);

        for (i = 0; i < N / 2 + 1; i++) {
            outR[i] = cabsf(out[i]) / (float)(N * SHRT_MAX);
            outR[i] = 2 * cbrt(outR[i]);
            outR[i] = smoothstepf(0, 1, outR[i] * 1.5f - 0.5f);
        }
    }

    // OpenGL|ES
    {
        float bla[136];
        int i;

        for (i = 0; i < 68; i++) {
            bla[2 * i] = clampf(outL[i], 0, 1);
        }

        for (i = 0; i < 68; i++) {
            bla[2 * i + 1] = clampf(outR[i], 0, 1);
        }

        commitData(bla);
    }
}


static char prevTitle[256] = {'\0'};
static char prevArtist[256] = {'\0'};
static char prevAlbum[256] = {'\0'};
static char *prevArtwork = NULL;
static size_t prevArtworkSize = 0;

static void updateNowPlaying() {
    bool update = false;

    if (strcmp(s_metaTitle, prevTitle) != 0) {
        update = true;
    }
    
    if (strcmp(s_metaArtist, prevArtist) != 0) {
        update = true;
    }
    
    if (strcmp(s_metaAlbum, prevAlbum) != 0) {
        update = true;
    }

    if (update) {
        if (s_metaTitle) {
            strncpy(prevTitle, s_metaTitle, 255);
        } else {
            prevTitle[0] = '-';
            prevTitle[1] = '\0';
        }

        if (s_metaArtist) {
            strncpy(prevArtist, s_metaArtist, 255);
        } else {
            prevArtist[0] = '-';
            prevArtist[1] = '\0';
        }

        if (s_metaAlbum) {
            strncpy(prevAlbum, s_metaAlbum, 255);
        } else {
            prevAlbum[0] = '-';
            prevAlbum[1] = '\0';
        }

        const char *title = (prevTitle[0]) ? prevTitle : "-";
        const char *artist = (prevArtist[0]) ? prevArtist : "-";
        const char *album = (prevAlbum[0]) ? prevAlbum : "-";
        char nowPlaying[2048];
        snprintf(nowPlaying, 2048, "%s\n%s\n%s", title, artist, album);

        setString(nowPlaying);
        setArtwork(NULL, 0);
    }

    if ((prevArtworkSize != s_metaArtworkSize) || (s_metaArtwork && prevArtwork && (strncmp(s_metaArtwork, prevArtwork, 128) != 0))) {
        memcpy(prevArtwork, s_metaArtwork, s_metaArtworkSize);
        prevArtworkSize = s_metaArtworkSize;
        setArtwork(prevArtwork, prevArtworkSize);
        update = true;
    }

    if (update) {
        if (s_metaTitle) {
            strncpy(prevTitle, s_metaTitle, 255);
        } else {
            prevTitle[0] = '-';
            prevTitle[1] = '\0';
        }

        if (s_metaArtist) {
            strncpy(prevArtist, s_metaArtist, 255);
        } else {
            prevArtist[0] = '-';
            prevArtist[1] = '\0';
        }

        if (s_metaAlbum) {
            strncpy(prevAlbum, s_metaAlbum, 255);
        } else {
            prevAlbum[0] = '-';
            prevAlbum[1] = '\0';
        }

        const char *title = (prevTitle[0]) ? prevTitle : "-";
        const char *artist = (prevArtist[0]) ? prevArtist : "-";
        const char *album = (prevAlbum[0]) ? prevAlbum : "-";
        char nowPlaying[2048];
        snprintf(nowPlaying, 2048, "%s\n%s\n%s", title, artist, album);

        setString(nowPlaying);
    }
}


int Fs;
long long starttime, samples_played;


static void help(void) {
    puts("no options available");
}


static int init2(int argc, char **argv) {
    optind = 1; // optind=0 is equivalent to optind=1 plus special behaviour
    argv--;     // so we shift the arguments to satisfy getopt()
    argc++;

    // some platforms apparently require optreset = 1; - which?
    int opt;

    while ((opt = getopt(argc, argv, "")) > 0) {
        switch (opt) {
            case 'b':
                break;

            default:
                help();
                die("Invalid audio option -%c specified", opt);
        }
    }

    if (optind < argc) {
        die("Invalid audio argument: %s", argv[optind]);
    }


    buffer = malloc(sizeof(short) * N * 4);


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


    // OpenGL
    bcm_host_init();

    TV_DISPLAY_STATE_T displayState;
    int result = vc_tv_get_display_state(&displayState);
    assert(result == 0);
    s_screenWidth = displayState.display.hdmi.width;
    s_screenHeight = displayState.display.hdmi.height;
    s_screenFrameRate = displayState.display.hdmi.frame_rate;
    printf("%d x %d @ %d\n", s_screenWidth, s_screenHeight, s_screenFrameRate);

    int rc = pthread_create(&s_renderingThread, NULL, glThread, NULL);
    assert(0 == rc);

#ifdef CONFIG_METADATA

    if (config.metadata_enabled) {
        rc = pthread_create(&s_metaDataThread, NULL, metaDataThread, NULL);
        assert(0 == rc);
        prevArtwork = malloc(ARTWORK_SIZE_MAX);
        s_metaArtwork = malloc(ARTWORK_SIZE_MAX);
        assert(prevArtwork);
    }

#endif

    return 0;
}



static void deinit2(void) {
    fftwf_destroy_plan(p);
    fftwf_free(in);
    fftwf_free(out);
    free(outL);
    free(outR);
    free(buffer);

    s_renderingThreadAlive = false;
    pthread_join(s_renderingThread, NULL);

#ifdef CONFIG_METADATA

    if (config.metadata_enabled) {
        free(s_metaArtwork);
        free(prevArtwork);
        pthread_join(s_metaDataThread, NULL);
    }

#endif

    bcm_host_deinit();
}



static void start(int sample_rate, int sample_format) {
    Fs = sample_rate;
    starttime = 0;
    samples_played = 0;
    printf("OpenGL|ES output started at Fs=%d Hz\n", sample_rate);
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

    updateNowPlaying();
}


static void stop(void) {
    setIdle(true);
    printf("OpenGL|ES stopped\n");
}


audio_output audio_gl = {
    .name = "gl",
    .help = &help,
    .init = &init2,
    .deinit = &deinit2,
    .start = &start,
    .stop = &stop,
    .flush = &flush,
    .delay = NULL,
    .play = &play,
    .volume = NULL,
    .parameters = NULL,
    .mute = NULL
};
