//
//  FlyingCubes.h
//  GLESCompute
//
//  Created by Michael Kwasnicki on 06.12.13.
//
//

#ifndef GLESCompute_Arabesque_h
#define GLESCompute_Arabesque_h

#include <stdbool.h>
#include <stddef.h>

void init(const int in_WIDTH, const int in_HEIGHT);
void deinit(void);
void update(void);
void draw(void);

void commitData(const float in_DATA[]);
void setIdle(const bool in_IDLE);
void setString(const char * const in_STRING);
void setArtwork(const void * const in_RAW_IMAGE, const size_t in_RAW_IMAGE_SIZE);

#endif
