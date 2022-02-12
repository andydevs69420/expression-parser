/*
 *   Copyright (c) 2022 
 *   All rights reserved.
 */


#ifndef TOKENIZER_H
#include "tokenizer.h"
#endif


void Tokenize(State* S,char *expr)
{
    S->expression = expr;

    if (strlen(S->expression) <= 0)
    {
        S->tokenizer.lookup = '$';
        S->tokenizer.isfeof = 1;
    }
    else
    {
        S->tokenizer.lookup = S->expression[0];
        S->tokenizer.isfeof = 0;
    }

    S->tokenizer.cursor = 0;
    S->tokenizer.line   = 1;
    S->tokenizer.column = 1;

    begin(S);
}

void forward(State *S)
{
    if (S->tokenizer.lookup == 0x0a)
    {
        S->tokenizer.line++;
        S->tokenizer.column = 1;
    }
    else
    {
        S->tokenizer.column++;
    }

    if (++S->tokenizer.cursor < strlen(S->expression)) 
    {
        S->tokenizer.lookup = S->expression[0];
        S->tokenizer.isfeof = 0;
    }
    else
    {
        S->tokenizer.isfeof = 1;
    }
}

void begin(State *S)
{
    while (!iseof(S))
    {
        if (isignoreable(S))
            ignore(S);
    }
}

int isignoreble(State *S)
{
    char ch = S->tokenizer.lookup;
    return (
        ch == 0x00 ||
        ch == 0x08 ||
        ch == 0x09 ||
        ch == 0x0a ||
        ch == 0x0d ||
        ch == 0x20
    );
}

void ignore(State *S)
{
    while (!iseof(S) && isignoreable(S))
        forward(S);
    
}

int iseof(State *S)
{
    return S->tokenizer.isfeof;
}