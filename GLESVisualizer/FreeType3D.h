//
//  FreeType3D.h
//  FreeType
//
//  Created by Michael Kwasnicki on 25.01.15.
//
//

#ifndef __FreeType__FreeType3D__
#define __FreeType__FreeType3D__



#include <stdbool.h>
#include <stdint.h>



typedef struct {
    uint16_t charCode;                  // only the first 65536 unicode glyphs
    uint16_t posX, posY;                // assuming a font map does not exceed 65536 x 65536 px
    int16_t bitMapLeft, bitMapTop;
    uint16_t bitMapWidth, bitMapHeight; // assuming a single glyph does not exceed 65536 x 65536 px
    uint16_t advance;
} ft3dFontMap_t;


typedef struct RGBA_s {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} RGBA_t;


typedef struct {
    float x, y;
    uint16_t s, t;
    RGBA_t color;
} ft3dVertex_t;



void ft3dStringToVertexArray(ft3dVertex_t **out_vertices, uint16_t *out_numVertex, float *out_width, const ft3dFontMap_t *in_FONT_MAP, const int in_FONT_MAP_SIZE, const char *in_STRING,
                             const float in_X, const float in_Y, const float in_LINE_SPACING, const float in_SCALE, const RGBA_t in_COLOR, const bool in_TOP_DOWN);



#endif /* defined(__FreeType__FreeType3D__) */
