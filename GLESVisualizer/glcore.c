//
//  glcore.c
//  GLESCompute
//
//  Created by Michael Kwasnicki on 01.12.13.
//
//

#include "glcore.h"

#include "Arabesque.h"
#include "globaltime.h"

#include <GLES2/gl2.h>
#include <stdio.h>
#include <stdlib.h>


void initGL(const int in_WIDTH, const int in_HEIGHT) {
    puts((char *)glGetString(GL_EXTENSIONS));

    timeReset();
    init(in_WIDTH, in_HEIGHT);
}


void drawGL() {
    timeTick();
    update();
    draw();
}


void destroyGL() {
    deinit();
}
