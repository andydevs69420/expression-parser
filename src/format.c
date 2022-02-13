/*
 *   Copyright (c) 2022 
 *   All rights reserved.
 */


#ifndef FORMAT_H
#include "format.h"
#endif

char *format(char *_fmt,...)
{
    char *buffer;
    buffer = (char*) malloc(sizeof(char) * MAX_BUFFER);

    va_list lst;
    va_start(lst,_fmt);
    vsnprintf(buffer,MAX_BUFFER,_fmt,lst);
    va_end(lst);

    return buffer;
}