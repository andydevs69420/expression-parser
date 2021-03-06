/*
 *   Copyright (c) 2022 
 *   All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "state.h"

#ifndef TOKENIZER_H
#define TOKENIZER_H

void Tokenizer(State*,char*);
void forward(State*);
Token getNext(State*);
int isignoreable(State*);
void ignore(State*);
int isidentifier(State*);
Token identifier(State*);
int isdigit(State*);
Token number(State*);
int isoperator(State*);
Token operator(State*);
int iseof(State*);
Token _feof(State*);
void error(State*);
char *getPos(State*,Token tok);

#endif
