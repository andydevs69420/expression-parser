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
typedef struct ATOM_NODE Flt_node;
typedef struct CALL_NODE Call_node;
typedef struct CALL_PARAM Call_param;
typedef struct EXPRESSION_NODE Exponential_node;
typedef struct EXPRESSION_NODE Arithmetic_node;
typedef struct UNARY_NODE Unary_node;

enum NodeType
{
    NIDN  = 0,
    NINT  = 1,
    NFLT  = 2,
    NCALL = 3,
    NEXP  = 4,
    NUNA  = 5,
    NARI  = 6
};

typedef struct NODE
{
    int type;
    Iden_node *identifier;
    Int_node *integer;
    Flt_node *floatn;
    Call_node *call;
    Exponential_node *expn;
    Unary_node *unary;
    Arithmetic_node *arithmetic;
} Node ;

typedef struct ATOM_NODE
{
    Token value;
} Iden_node, Int_node ,Flt_node ;

typedef struct CALL_NODE
{
    Node *object;
    Call_param *params;
} Call_node ;

typedef struct CALL_PARAM
{
    int pcount;
    Node **parameters;
} Call_param ;

typedef struct EXPRESSION_NODE
{
    Token op;
    Node *lhs;
    Node *rhs;
} Exponential_node , Arithmetic_node;

typedef struct UNARY_NODE
{
    Token op;
    Node *expr;
} Unary_node ;


void Parser(State*,char*);
int match(State*,char*);
void expect(State*,char*);
Node *parse(State*);
Node *atom(State*);
Node *iden(State*);
Node *integer(State*);
Node *pnumber(State*);
Node *call(State*);
Call_param *call_params(State*);
Node *exponential(State*);
Node *factor(State*);
Node *multiplicative(State*);
Node *addetive(State*);
Node *expression(State*);
Node *null_safe_expr(State*);

int parseeof(State*);

#endif