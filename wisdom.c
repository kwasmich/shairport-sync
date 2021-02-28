//
//  wisdom.c
//  shairport-sync
//
//  Created by Michael Kwasnicki on 28.02.21.
//  Copyright © 2021 Michael Kwasnicki. All rights reserved.
//

#include <complex.h>
#include <fftw3.h>
#include <stdio.h>
#include <string.h>



static const int N = 1024;
static fftwf_plan p;
static float *in = NULL;
static fftwf_complex *out = NULL;



static void usage(const char * const in_EXECUTABLE) {
    puts("usage:");
    printf("%s <mode>\n", in_EXECUTABLE);
    puts("    <mode>              the search mode for the optimal FFT strategy:");
    puts("                        'FFTW_ESTIMATE'   fastest but worst runtime perfomrance.");
    puts("                        'FFTW_MEASURE'    fast");
    puts("                        'FFTW_PATIENT'    slow");
    puts("                        'FFTW_EXHAUSTIVE' slowest but best runtime perfomrance.");
}



int main(int argc, char **argv) {
    if (argc != 2) {
        usage(argv[0]);
        return -1;
    }

    unsigned flags = 0;

    if (strncmp(argv[1], "FFTW_ESTIMATE", strlen("FFTW_ESTIMATE")) == 0) {
        flags = FFTW_ESTIMATE;
    } else if (strncmp(argv[1], "FFTW_MEASURE", strlen("FFTW_MEASURE")) == 0) {
        flags = FFTW_MEASURE;
    } else if (strncmp(argv[1], "FFTW_PATIENT", strlen("FFTW_PATIENT")) == 0) {
        flags = FFTW_PATIENT;
    } else if (strncmp(argv[1], "FFTW_EXHAUSTIVE", strlen("FFTW_EXHAUSTIVE")) == 0) {
        flags = FFTW_EXHAUSTIVE;
    } else {
        usage(argv[0]);
        return -1;
    }

    in = (float *) fftwf_malloc(sizeof(float) * N);
    out = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * N);
    p = fftwf_plan_dft_r2c_1d(N, in, out, flags);
    puts(fftwf_export_wisdom_to_string());
    return 0;
}
