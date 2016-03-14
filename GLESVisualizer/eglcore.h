//
//  eglcore.h
//  GLESCompute
//
//  Created by Michael Kwasnicki on 01.12.13.
//
//

#ifndef GLESCompute_eglcore_h
#define GLESCompute_eglcore_h

#include <stdint.h>

void initEGL(const uint32_t in_DISPLAY_NUMBER, const uint32_t in_X, const uint32_t in_Y, const uint32_t in_W, const uint32_t in_H);
void drawEGL(void);
void destroyEGL(void);

#endif
