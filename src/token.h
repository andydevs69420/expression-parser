/*
 *   Copyright (c) 2022 
 *   All rights reserved.
 */

#ifndef TOKEN_H
#define TOKEN_H

enum TokenType
{
    IDN = 0,
    INT = 1,
    FLT = 2,
    OPT = 3,
    ERR = 4,
    END = 5,
};

typedef struct TOKEN
{
    int type;
    int line;
    int colm;
    char *token;
} Token ;

#endif