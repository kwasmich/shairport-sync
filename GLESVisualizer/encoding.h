//
//  encoding.h
//  FreeType
//
//  Created by Michael Kwasnicki on 13.03.15.
//  Copyright 2015 __MyCompanyName__. All rights reserved.
//


#ifndef encoding_h
#define encoding_h


#include <string.h>
#include <wchar.h>


void utf8ToWChar(wchar_t **out_wCharString, char const *const in_UTF8_STRING);
void wCharToUTF8(char **out_utf8String, wchar_t const *const in_WCHAR_STRING);


#endif
