/*
 *   Copyright (c) 2022 
 *   All rights reserved.
 */


#ifndef TOKENIZER_H
#include "tokenizer.h"
#endif


#define newBuilder() int size = 1; \
                     char *builder = malloc(sizeof(char));

#define pushChar(_char) builder[size - 1] = _char; \
                        builder = (char*) realloc(builder,sizeof(char) * ++size); \
                        builder[size - 1] = '\0'; \

#define getBuilder() builder; 


void Tokenizer(State* S,char *expr)
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
        S->tokenizer.lookup = S->expression[S->tokenizer.cursor];
        S->tokenizer.isfeof = 0;
    }
    else
    {
        S->tokenizer.isfeof = 1;
    }
}

Token getNext(State *S)
{
    while (!iseof(S))
    {
        if (isignoreable(S))
            ignore(S);
        else if (isidentifier(S))
            return identifier(S);
        else if (isdigit(S))
            return number(S);
        else if (isoperator(S))
            return operator(S);
        else 
        {
            Token tok;
            tok.type  = ERR;
            tok.line  = S->tokenizer.line;
            tok.colm  = S->tokenizer.column;
            char ch[] = {S->tokenizer.lookup,'\0'}; // kapoy na
            tok.token = ch;

            printf(
                "SyntaxError: invalid token \"%c\"\n\n%s\n\n",
                S->tokenizer.lookup,
                getPos(S,tok)
            );
            exit(1);
        }
    }
    return _feof(S);
}

int isignoreable(State *S)
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

int isidentifier(State *S)
{
    char ch = S->tokenizer.lookup;
    return (
        (ch >= 0x61 && ch <= 0x7a) ||
        (ch >= 0x41 && ch <= 0x5a) ||
        (ch == 0x5f)
    );
}

Token identifier(State *S)
{
    Token tok;
    tok.type = IDN;
    tok.line = S->tokenizer.line;
    tok.colm = S->tokenizer.column;

    newBuilder();

    while 
    (
        !iseof(S) && 
        (
            isidentifier(S) || 
            (strlen(builder) > 0 && isdigit(S))
        )
    )
    {
        pushChar(S->tokenizer.lookup);
        forward(S);
    }
    tok.token = getBuilder();
    return tok;
}

int isdigit(State *S)
{
    char ch = S->tokenizer.lookup;
    return (ch >= 0x30 && ch <= 0x39);
}

Token number(State *S)
{
    Token tok;
    tok.type = INT;
    tok.line = S->tokenizer.line;
    tok.colm = S->tokenizer.column;

    newBuilder();

    while (!iseof(S) && isdigit(S))
    {
        pushChar(S->tokenizer.lookup);
        forward(S);
    }
    tok.token = getBuilder();
    return tok;
}

int isoperator(State *S)
{
    char ch = S->tokenizer.lookup;
    switch (ch)
    {
        case '(':
        case ')':
        case '.':
        case '*':
        case '/':
        case '+':
        case '-':
        case ',': return 1;
        default : return 0;
    }
}

Token operator(State *S)
{
    Token tok;
    tok.type = OPT;
    tok.line = S->tokenizer.line;
    tok.colm = S->tokenizer.column;

    newBuilder();
    pushChar(S->tokenizer.lookup);
    tok.token = getBuilder();
    return tok;
}

int iseof(State *S)
{
    return S->tokenizer.isfeof;
}

Token _feof(State *S)
{
    Token tok;
    tok.type  = OPT;
    tok.line  = S->tokenizer.line;
    tok.colm  = S->tokenizer.column;
    tok.token = "eof";
    return tok;
}

char *getPos(State *S,Token tok)
{
    newBuilder();

    int idx;
    idx = 0;

    while (idx < strlen(S->expression))
    {
        pushChar(S->expression[idx])
        idx++;
    }

    pushChar('\n');
    
    for (idx = 0; idx < strlen(S->expression);idx++)
    {
        if ((idx+1) >= tok.colm && (idx+1) <= (tok.colm + (strlen(tok.token)-1)))
        {
            pushChar('^');
        }
        else
        {
            pushChar(' ');
        }
    }


    return getBuilder();
}
