//
//  simpleJPEG.h
//  SimpleJPEG
//
//  Created by Michael Kwasnicki on 20.10.15.
//  Copyright © 2015 Michael Kwasnicki. All rights reserved.
//

#ifndef simpleJPEG_h
#define simpleJPEG_h


#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

bool jpegIsJPEG(const uint8_t * const in_JPEG_DATA);
bool jpegDecode(uint8_t ** const out_image, uint32_t * const out_width, uint32_t * const out_height, uint32_t * const out_numChannels, const uint8_t * const in_JPEG_DATA, const size_t in_JPEG_SIZE, const bool in_FLIP_Y);
bool jpegRead(uint8_t ** const out_image, uint32_t * const out_width, uint32_t * const out_height, uint32_t * const out_numChannels, const char * const in_FILE_NAME, const bool in_FLIP_Y);
bool jpegWrite(const char * const in_FILE_NAME, const uint32_t in_QUALITY, const uint8_t * const in_IMAGE, const uint32_t in_WIDHT, const uint32_t in_HEIGHT, const uint32_t in_NUM_CHANNELS);
void jpegFree(uint8_t ** const in_out_image);

#ifdef __cplusplus
}
#endif


#endif /* simpleJPEG_h */
