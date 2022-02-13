/*
 *   Copyright (c) 2022 
 *   All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "state.h"
#include "eval.h"

int main()
{
    
    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #elif defined(linux)
        system("clear");
    #endif

    printf("******************************************************************\n");
    printf("* title : expression-parser                                      *\n");
    printf("* author: Philipp Andrew Roa Redondo                             *\n");
    printf("* github: https://github.com/andydevs69420/expression-parser.git *\n");
    printf("******************************************************************\n");
    printf("Ctrl + c or type \"quit\" to exit.\n\n");
    State *state = (State*) malloc(sizeof(State));

    char *expr;

    do 
    {
        printf("Prelude:>> ");
        scanf("%[^\n]s",expr);
        fflush(stdin);
        Result res = eval(state,expr);
        printf("%s\n",res.value);
    } while (strcmp(expr,"quit") != 0);

    free(state);
    return 0;
}