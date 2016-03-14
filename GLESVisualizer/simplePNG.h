//
//  PNG.h
//  PNG
//
//  Created by Michael Kwasnicki on 19.03.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#ifndef __PNG_H__
#define __PNG_H__


#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

bool pngIsPNG(const uint8_t * const in_PNG_DATA);
bool pngDecode(uint8_t **out_image, uint32_t *out_width, uint32_t *out_height, uint32_t *out_channels, const uint8_t * const in_PNG_DATA, const size_t in_PNG_SIZE, const bool in_FLIP_Y);
int pngCheck(const char * const in_FILE);
bool pngRead(const char * const in_FILE, const bool in_FLIP_Y, uint8_t ** const out_image, uint32_t * const out_width, uint32_t * const out_height, uint32_t * const out_channels) ;
bool pngWrite(const char * const in_FILE_NAME, uint8_t * const in_IMAGE, const uint32_t in_WIDTH, const uint32_t in_HEIGHT, const uint32_t in_CHANNELS);
void pngFree(uint8_t ** const in_out_image);

#ifdef __cplusplus
}
#endif


#endif //__PNG_H__
