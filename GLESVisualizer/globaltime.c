//
//  globaltime.c
//  GlobalTime
//
//  Created by Michael Kwasnicki on 25.01.15.
//  Copyright (c) 2015 Michael Kwasnicki. All rights reserved.
//

#include "globaltime.h"

#include <stddef.h>
#include <sys/time.h>


static struct timeval s_relativeZero;
static float s_time = 0;
static float s_timeDelta = 0;



void timeReset() {
    gettimeofday(&s_relativeZero, NULL);
    s_time = 0;
}



void timeTick() {
    struct timeval now, delta;
    gettimeofday(&now, NULL);
    timersub(&now, &s_relativeZero, &delta);
    float before = s_time;
    s_time = (float)(delta.tv_sec) + (float)(delta.tv_usec) * 0.000001f;
    s_timeDelta = s_time - before;
    //    sprintf( "%f\n", s_time );
}



void timeSet(const float in_TIME) {
    s_time = in_TIME;
    s_timeDelta = 0;
}



float timeGet() {
    return s_time;
}



float timeDelta() {
    return s_timeDelta;
}
