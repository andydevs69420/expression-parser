/*
 *   Copyright (c) 2022 
 *   All rights reserved.
 */


#ifndef PARSER_H
#include "parser.h"
#endif


#define nextToken() S->parser.ctok = getNext(S);

void Parser(State *S,char *expr)
{
    Tokenizer(S,expr);
    
    S->parser.ctok = getNext(S);

}

int match(State *S,char *pattern)
{
    return (strcmp(S->parser.ctok.token,pattern) == 0);
}

void expect(State *S,char *pattern)
{
    if (match(S,pattern))
    {
        S->parser.ctok = getNext(S);
    }
    else
    {
        printf(
            "SyntaxError: expected token \"%s\", got \"%s\"\n\n%s\n\n",
            pattern,
            S->parser.ctok,
            getPos(S,S->parser.ctok)
        );
        exit(1);
    }
}

void parse(State *S)
{
    while (S->parser.ctok.type != END)
    {

    }
}


Node *atom(State *S)
{
    if (S->parser.ctok.type == NIDN)
    {
        return iden(S);
    }
    else if (S->parser.ctok.type == NINT)
    {
        return number(S);
    }
}

Node *ident(State *S)
{
    Node *node;
    node = (Node*) malloc(sizeof(Node));

    node->type = NIDN;
    node->identifier = (Iden_node*) malloc(sizeof(Iden_node));
    node->identifier->value = S->parser.ctok;

    nextToken();

    return node;
}

Node *integer(State *S)
{
    Node *node;
    node = (Node*) malloc(sizeof(Node));

    node->type = NINT;
    node->integer = (Int_node*) malloc(sizeof(Int_node));
    node->integer->value = S->parser.ctok;

    nextToken();
    
    return node;
}

Node *number(State *S)
{
    Node *intnode = integer(S);
    if 
    (!
        (
            match(S,".") ||
            match(S,"e") ||
            match(S,"E")
        )
    ) 
        return intnode;

    Token tok = S->parser.ctok;
    nextToken();

    if (S->parser.ctok.type!= INT)
    {
        printf(
            "SyntaxError: unexpected token \"%s\", remove these token\n\n%s\n\n",
            tok.token,
            getPos(S,tok)
        );
        exit(1);
    }

    Node *next = integer(S);

    Node *new = (Node*) malloc(sizeof(Node));
    new->type = NFLT;
    new->floatn = (Flt_node*) malloc(sizeof(Flt_node));

    int size = strlen(intnode->integer->value.token) +
               strlen(tok.token) +
               strlen(next->integer->value.token);

    new->floatn->value = (char*) malloc(sizeof(char) * (size + 1));

    strcat(new->floatn->value,intnode->integer->value.token);
    strcat(new->floatn->value,tok.token);
    strcat(new->floatn->value,next->integer->value.token);

    new->floatn->value[size-1] = '\0';

    nextToken();

    free(intnode);
    free(next);

    return new;
}

Node *call(State *S)
{
    Node *atomr = atom(S);

    if (!match(S->parser.ctok.token,"(")) return atomr;

    // call
    if (atomr->type != NIDN)
    {
        char *typename;
        Token ref;

        switch (atomr->type)
        {
            case NINT: 
            {
                typename = "integer";
                ref = atomr->integer->value;
                break;
            }
            case NFLT: 
            {
                typename = "float";
                ref = atomr->floatn->value;
                break;
            }
        }

        printf(
            "SyntaxError: \"%s\" is not callable. remove these token\n\n%s\n\n",
            typename,
            getPos(S,ref)
        );
        exit(1);
    }
}

