/*
 *   Copyright (c) 2022 
 *   All rights reserved.
 */

#include <string.h>
#include <stdlib.h>
#include "state.h"

#ifndef TOKENIZER_H
#define TOKENIZER_H

void Tokenize(State*,char*);
void forward(State*);
void begin(State*);
int isignoreable(State*);
void ignore(State*);
int iseof(State*);

#endif
