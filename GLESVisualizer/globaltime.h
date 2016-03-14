//
//  globaltime.h
//  GlobalTime
//
//  Created by Michael Kwasnicki on 25.01.15.
//  Copyright (c) 2015 Michael Kwasnicki. All rights reserved.
//

#ifndef __GlobalTime__globaltime__
#define __GlobalTime__globaltime__


void timeReset(void);
void timeTick(void);
void timeSet(const float in_TIME);
float timeGet(void);
float timeDelta(void);


#endif /* defined(__GlobalTime__globaltime__) */
