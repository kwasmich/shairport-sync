//
//  FreeType3D.c
//  FreeType
//
//  Created by Michael Kwasnicki on 25.01.15.
//
//

#include "FreeType3D.h"


#include "encoding.h"


#include <assert.h>
#include <iso646.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>



static int compareFontMapEntry(const void *in_A, const void *in_B) {
    return ((ft3dFontMap_t *)in_A)->charCode - ((ft3dFontMap_t *)in_B)->charCode;
}



void ft3dStringToVertexArray(ft3dVertex_t **out_vertices, uint16_t *out_numVertex, float *out_width, const ft3dFontMap_t *in_FONT_MAP, const int in_FONT_MAP_SIZE, const char *in_STRING,
                             const float in_X, const float in_Y, const float in_LINE_SPACING, const float in_SCALE, const RGBA_t in_COLOR, const bool in_TOP_DOWN) {
    wchar_t *wStr = NULL;
    utf8ToWChar(&wStr, in_STRING);

    size_t numChars = wcslen(wStr);
    ft3dVertex_t *vertices = malloc(numChars * 6 * sizeof(ft3dVertex_t));
    int cnt = 0;
    float penX = in_X;
    float penY = in_Y;
    size_t c, i;

    for (c = 0; c < numChars; c++) {
        size_t idx = wStr[c];
        ft3dFontMap_t key = { .charCode = idx };
        const ft3dFontMap_t *g = bsearch(&key, in_FONT_MAP, in_FONT_MAP_SIZE, sizeof(ft3dFontMap_t), compareFontMapEntry);

        if (not g) {
            g = &in_FONT_MAP[0];
        }

        if (idx == '\n') {
            if (in_TOP_DOWN) {
                penX -= in_LINE_SPACING;
                penY = in_Y;
            } else {
                penX = in_X;
                penY -= in_LINE_SPACING;
            }

            continue;
        }

        vertices[6 * cnt + 1].x = (g->bitMapLeft) * in_SCALE;
        vertices[6 * cnt + 1].y = (g->bitMapTop) * in_SCALE;
        vertices[6 * cnt + 1].s = g->posX;
        vertices[6 * cnt + 1].t = g->posY;
        vertices[6 * cnt + 1].color = in_COLOR;
        vertices[6 * cnt + 2].x = (g->bitMapLeft) * in_SCALE;
        vertices[6 * cnt + 2].y = (g->bitMapTop - g->bitMapHeight) * in_SCALE;
        vertices[6 * cnt + 2].s = g->posX;
        vertices[6 * cnt + 2].t = g->posY + g->bitMapHeight;
        vertices[6 * cnt + 2].color = in_COLOR;
        vertices[6 * cnt + 3].x = (g->bitMapLeft + g->bitMapWidth) * in_SCALE;
        vertices[6 * cnt + 3].y = (g->bitMapTop) * in_SCALE;
        vertices[6 * cnt + 3].s = g->posX + g->bitMapWidth;
        vertices[6 * cnt + 3].t = g->posY;
        vertices[6 * cnt + 3].color = in_COLOR;
        vertices[6 * cnt + 4].x = (g->bitMapLeft + g->bitMapWidth) * in_SCALE;
        vertices[6 * cnt + 4].y = (g->bitMapTop - g->bitMapHeight) * in_SCALE;
        vertices[6 * cnt + 4].s = g->posX + g->bitMapWidth;
        vertices[6 * cnt + 4].t = g->posY + g->bitMapHeight;
        vertices[6 * cnt + 4].color = in_COLOR;

        vertices[6 * cnt + 0] = vertices[6 * cnt + 1];
        vertices[6 * cnt + 5] = vertices[6 * cnt + 4];

        if (in_TOP_DOWN) {
            for (i = 0; i < 6; i++) {
                float x = vertices[6 * cnt + i].x;
                float y = vertices[6 * cnt + i].y;
                vertices[6 * cnt + i].x =  y;
                vertices[6 * cnt + i].y = -x;
            }
        }

        for (i = 0; i < 6; i++) {
            vertices[6 * cnt + i].x += penX;
            vertices[6 * cnt + i].y += penY;
        }

        if (in_TOP_DOWN) {
            penY -= g->advance * in_SCALE;
        } else {
            penX += g->advance * in_SCALE;
        }

        cnt++;
    }

    assert(*out_vertices == NULL);
    *out_vertices = vertices;
    *out_numVertex = cnt * 6;

    if (in_TOP_DOWN) {
        *out_width = in_Y - penY;
    } else {
        *out_width = penX - in_X;
    }
}
