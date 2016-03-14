//
//  colorspace.c
//  RGB2HSL
//
//  Created by Michael Kwasnicki on 08.12.13.
//
//

#include "colorspace.h"

//#include <iso646.h>
#include <math.h>
//#include <stdio.h>



hsl_t colorspaceConvertRGB2HSL(const rgb8_t in_RGB) {
    u_int8_t min = (in_RGB.r < in_RGB.g) ? in_RGB.r : in_RGB.g;
    min = (in_RGB.b < min) ? in_RGB.b : min;
    u_int8_t max = (in_RGB.r > in_RGB.g) ? in_RGB.r : in_RGB.g;
    max = (in_RGB.b > max) ? in_RGB.b : max;

    float hue = 0;
    float saturation;
    float lightness;
    const float inverseDelta = 1.0f / (max - min);

    if (min == max) {
        hue = 0;
    } else if (in_RGB.r == max) {
        hue = 60 * (in_RGB.g - in_RGB.b) * inverseDelta;
    } else if (in_RGB.g == max) {
        hue = 120 + 60 * (in_RGB.b - in_RGB.r) * inverseDelta;
    } else if (in_RGB.b == max) {
        hue = 240 + 60 * (in_RGB.r - in_RGB.g) * inverseDelta;
    }

    if (hue < 0) {
        hue += 360;
    }

    if (min == max) {
        saturation = 0;
    } else {
        saturation = (max - min) / (255.0f - fabsf(max + min - 255));
    }

    lightness = (max + min) * 0.5;

    hsl_t hsl;
    hsl.h = hue;
    hsl.s = saturation;
    hsl.l = lightness / 255.0f;
    return hsl;
}



rgb8_t colorspaceConvertHSL2RGB(const hsl_t in_HSL) {
    const float c = (1 - fabsf(fmaf(2, in_HSL.l, -1))) * in_HSL.s;
    const float h = fmodf(in_HSL.h, 360.0f) / 60.0f;
    const int hh = h;
    const float x = c * (1 - fabsf(fmodf(h, 2) - 1));
    float r, g, b;

    switch (hh) {
        default:
        case 0:
            r = c;
            g = x;
            b = 0;
            break;

        case 1:
            r = x;
            g = c;
            b = 0;
            break;

        case 2:
            r = 0;
            g = c;
            b = x;
            break;

        case 3:
            r = 0;
            g = x;
            b = c;
            break;

        case 4:
            r = x;
            g = 0;
            b = c;
            break;

        case 5:
            r = c;
            g = 0;
            b = x;
            break;
    }

    const float m = in_HSL.l - c * 0.5;
    r += m;
    g += m;
    b += m;

    rgb8_t rgb;
    rgb.r = r * 255.0f + 0.5f;
    rgb.g = g * 255.0f + 0.5f;
    rgb.b = b * 255.0f + 0.5f;
    return rgb;
}
