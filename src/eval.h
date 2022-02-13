/*
 *   Copyright (c) 2022 
 *   All rights reserved.
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "state.h"
#include "parser.h"
#include "format.h"
#include "builtins.h"

#ifndef EVAL_H
#define EVAL_H

enum ResultType
{
    EINT   = 0,
    EFLOAT = 1,
    EUNARY = 2
};

typedef struct RESULT
{
    int type;
    char *typen;
    char *value;
} Result ;

Result eval(State*,char*);
Result evaluate(State*,Node*);
Result eval_iden(State*,Node*);
Result eval_int(State*,Node*);
Result eval_flt(State*,Node*);
Result eval_unary(State*,Node*);
Result eval_exponential(State*,Node*);
Result eval_arithmetic(State*,Node*);

#endif

