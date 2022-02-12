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
        nextToken();
    }
    else
    {
        printf(
            "SyntaxError: expected token \"%s\", got \"%s\"\n\n%s\n\n",
            pattern,
            S->parser.ctok.token,
            getPos(S,S->parser.ctok)
        );
        exit(1);
    }
}

void parse(State *S)
{
    while (!parseeof(S))
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
    return NULL;
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

    char *token = (char*) malloc(sizeof(char) * (size + 1));

    strcat(token,intnode->integer->value.token);
    strcat(token,tok.token);
    strcat(token,next->integer->value.token);

    token[size-1] = '\0';

    Token newtok;
    newtok.type  = FLT;
    newtok.line  = intnode->integer->value.line;
    newtok.colm  = intnode->integer->value.colm;
    newtok.token = token;

    new->floatn->value = newtok;

    nextToken();

    free(intnode);
    free(next);

    return new;
}

Node *call(State *S)
{
    Node *atomr = atom(S);

    if (!match(S->parser.ctok.token,"(")) return atomr;

    // no recursive all
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
            "TypeError: \"%s\" is not callable. remove these token\n\n%s\n\n",
            typename,
            getPos(S,ref)
        );
        exit(1);
    }

    Node *calln = (Node*) malloc(sizeof(Node));
    calln->type = NCALL;
    calln->call = (Call_node*) malloc(sizeof(Call_node));
    calln->call->object = atomr;

    Token rpar = S->parser.ctok;

    expect(S,"(");

    calln->call->params = call_params(S);

    if (!match(S,")"))
    {
        printf(
            "SyntaxError: unclosed parenthesis \"%s\", remove these token\n\n%s\n\n",
            rpar.token,
            getPos(S,rpar)
        );

        printf(
            "expected token \"%s\", got \"%s\"\n\n%s\n\n",
            ")",
            S->parser.ctok.token,
            getPos(S,S->parser.ctok)
        );

        exit(1);

    }

    expect(S,")");

    return calln;
}

Call_param *call_params(State *S)
{
    Call_param *cparam;

    cparam->pcount = 0;
    cparam->parameters = (Node*) malloc(sizeof(Node));

    Node *param = expresson(S);
    if (param == NULL) return cparam;

    cparam->pcount++;
    cparam->parameters[cparam->pcount-1] = param;
    cparam->parameters = (Node**) realloc(
        cparam->parameters,
        sizeof(Node*) * (1+cparam->pcount)
    );


    while (!parseeof(S) && match(S,","))
    {
        Token comma = S->parser.ctok;

        if (match(S,","))
            nextToken();
        
        param = expression(S);

        if (param == NULL)
        {
            printf(
                "SyntaxError: remove extra \"%s\"\n\n%s\n\n",
                comma.token,
                getPos(S,S->parser.ctok)
            );
            exit(1);
        }

        cparam->pcount++;
        cparam->parameters[cparam->pcount-1] = param;
        cparam->parameters = (Node**) realloc(
            cparam->parameters,
            sizeof(Node*) * (1+cparam->pcount)
        );
    }
    return cparam;
}

Node *exponential(State *S)
{
    Node *node = call(S);
    if (node == NULL) return node;

    Node *newn = (Node*) malloc(sizeof(Node));
    newn->type = NEXP;
    newn->expn = (Exponential_node*) malloc(sizeof(Exponential_node));

    newn->expn->lhs = node;
    newn->expn->op  = S->parser.ctok;

    while (!parseeof(S) && match(S,"^")) // use xor(^) as exponential
    {
        if (match(S,"^"))
            nextToken();
        
        Node *rhs = exponential(S);
        
        if (rhs == NULL)
        {
            printf(
                "SyntaxError: expects \"expression\" after \"%s\", got \"%s\"\n\n%s\n\n",
                newn->expn->op.token,
                S->parser.ctok.token,
                getPos(S,S->parser.ctok)
            );
            exit(1);
        }

        newn->expn->rhs = rhs;
    }

    return newn;
}

int parseeof(State *S)
{
    return (S->parser.ctok.type == END);
}