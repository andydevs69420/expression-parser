/*
 *   Copyright (c) 2022 
 *   All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include "tokenizer.h"

#ifndef PARSER_H
#define PARSER_H

typedef struct ATOM_NODE Iden_node;
typedef struct ATOM_NODE Int_node;
typedef struct FLT_NODE Flt_node;

enum NodeType
{
    NIDN = 0,
    NINT = 1,
    NFLT = 2
};

typedef struct NODE
{
    int type;
    Iden_node *identifier;
    Int_node *integer;
    Flt_node *floatn;
} Node ;

typedef struct ATOM_NODE
{
    Token value;
} Iden_node, Int_node ;

typedef struct FLT_NODE
{
    char *value;
} Flt_node ;


void Parser(State*,char*);
int match(State*,char*);
void expect(State*,char*);
void parse(State*);

Node *atom(State*);
Node *iden(State*);
Node *integer(State*);
Node *number(State*);
Node *call(State*);

#endif