//
//  encoding.c
//  FreeType
//
//  Created by Michael Kwasnicki on 13.03.15.
//  Copyright 2015 __MyCompanyName__. All rights reserved.
//


#include "encoding.h"


#include <assert.h>
#include <iconv.h>
#include <stdint.h>
#include <stdlib.h>



void utf8ToWChar(wchar_t **out_wCharString, char const *const in_UTF8_STRING) {
    assert(*out_wCharString == NULL);

    char *utf8Ptr = (char *)in_UTF8_STRING;
    size_t utf8StringBytes = strlen(in_UTF8_STRING);
    size_t wCharStringBytes = sizeof(wchar_t) * utf8StringBytes;
    wchar_t *wCharString = malloc(wCharStringBytes);
    char *wCharPtr = (char *)wCharString;

    iconv_t c8t32 = iconv_open("WCHAR_T", "UTF-8");

    // dry run to determine wchar string length.
    size_t error = iconv(c8t32, &utf8Ptr, &utf8StringBytes, &wCharPtr, &wCharStringBytes);
    assert(error != (size_t)(-1));

    free(wCharString);
    utf8Ptr = (char *)in_UTF8_STRING;
    utf8StringBytes = sizeof(char) * strlen(in_UTF8_STRING);
    wCharStringBytes = sizeof(wchar_t) * utf8StringBytes - wCharStringBytes + sizeof(wchar_t);
    wCharString = malloc(wCharStringBytes);
    wCharPtr = (char *)wCharString;

    error = iconv(c8t32, &utf8Ptr, &utf8StringBytes, &wCharPtr, &wCharStringBytes);
    assert(error != (size_t)(-1));

    iconv_close(c8t32);

    assert(wCharStringBytes = sizeof(wchar_t));       //make sure the target buffer was long enough
    *(wchar_t *)(wCharPtr) = L'\0';                   //terminate the string with a NULL
    *out_wCharString = wCharString;
}



void wCharToUTF8(char **out_utf8String, wchar_t const *const in_WCHAR_STRING) {
    assert(*out_utf8String == NULL);

    char *wCharPtr = (char *)in_WCHAR_STRING;
    size_t wCharStringBytes = sizeof(wchar_t) * wcslen(in_WCHAR_STRING);
    size_t utf8StringBytes = wCharStringBytes;
    char *utf8String = malloc(utf8StringBytes);
    char *utf8Ptr = utf8String;

    _Static_assert(sizeof(wchar_t) == sizeof(uint32_t), "assuming 32bit unicode");

    iconv_t c32t8 = iconv_open("UTF-8", "WCHAR_T");

    // dry run to determine utf8 string length
    size_t error = iconv(c32t8, &wCharPtr, &wCharStringBytes, &utf8Ptr, &utf8StringBytes);
    assert(error != (size_t)(-1));

    free(utf8String);
    wCharPtr = (char *)in_WCHAR_STRING;
    wCharStringBytes = sizeof(wchar_t) * wcslen(in_WCHAR_STRING);
    utf8StringBytes = wCharStringBytes - utf8StringBytes + sizeof(char);
    utf8String = malloc(utf8StringBytes);
    utf8Ptr = utf8String;

    error = iconv(c32t8, &wCharPtr, &wCharStringBytes, &utf8Ptr, &utf8StringBytes);
    assert(error != (size_t)(-1));

    iconv_close(c32t8);

    assert(utf8StringBytes > 0);        //make sure the target buffer was long enough.
    *utf8Ptr = '\0';                    //terminate the string with a NULL
    *out_utf8String = utf8String;
}
