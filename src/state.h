/*
 *   Copyright (c) 2022 
 *   All rights reserved.
 */

#include "token.h"

#ifndef STATE_H
#define STATE_H

typedef struct SHARED_STATE
{
    char *expression;
    struct TOKENIZER_STATE
    {
        char lookup;
        int isfeof;
        int cursor;
        int line;
        int column;
    } tokenizer ;


    struct PARSER_STATE
    {

        Token ctok;
    } parser ;
    
} State ;


#endif
