//
//  colorspace.h
//  RGB2HSL
//
//  Created by Michael Kwasnicki on 08.12.13.
//
//

#ifndef RGB2HSL_colorspace_h
#define RGB2HSL_colorspace_h

#include <sys/types.h>

typedef struct {
    u_int8_t r;
    u_int8_t g;
    u_int8_t b;
} rgb8_t;

typedef struct {
    float h;
    float s;
    float l;
} hsl_t;


hsl_t colorspaceConvertRGB2HSL(const rgb8_t in_RGB);
rgb8_t colorspaceConvertHSL2RGB(const hsl_t in_HSL);

#endif
