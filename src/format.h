/*
 *   Copyright (c) 2022 
 *   All rights reserved.
 */

#include <stdarg.h>
#include <stdio.h>

#ifndef FORMAT_H
#define FORMAT_H

static int MAX_BUFFER = 4096;

char *format(char *_fmt,...)
{
    char *buffer;

    va_list lst;
    va_start(lst,_fmt);
    vsnprintf(buffer,MAX_BUFFER,_fmt,lst);
    va_end(lst);

    return buffer;
}

#endif

