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

    //  printf(
    //     "SyntaxError: invalid syntax \"%s\"\n\n%s\n\n",
    //     S->parser.ctok.token,
    //     getPos(S,S->parser.ctok)
    // );
    // exit(1);
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
            "\033[31mSyntaxError: expected token \"%s\", got \"%s\"\n\n%s\n\n\033[0m",
            pattern,
            S->parser.ctok.token,
            getPos(S,S->parser.ctok)
        );
        exit(1);
    }
}

Node *parse(State *S)
{
    Node *ast;
    while (!parseeof(S))
    {
        ast = null_safe_expr(S);
    }
    return ast;
}


Node *atom(State *S)
{
    if (S->parser.ctok.type == IDN)
    {
        return iden(S);
    }
    else if (S->parser.ctok.type == INT)
    {
        return pnumber(S);
    }
    return NULL;
}

Node *iden(State *S)
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

Node *pnumber(State *S)
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
            "\033[31mSyntaxError: unexpected token \"%s\", remove these token\n\n%s\n\n\033[0m",
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

    char *token = (char*) malloc(sizeof(char) * size);

    sprintf(
        token,
        "%s%s%s",
        intnode->integer->value.token,
        tok.token,
        next->integer->value.token
    );

    Token newtok;
    newtok.type  = FLT;
    newtok.line  = intnode->integer->value.line;
    newtok.colm  = intnode->integer->value.colm;
    newtok.token = token;

    new->floatn->value = newtok;

    free(intnode);
    free(next);

    return new;
}

Node *call(State *S)
{
    Node *atomr = atom(S);

    if (!match(S,"(")) return atomr;

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
            "\033[31mTypeError: \"%s\" is not callable\n\n%s\n\n\033[0m",
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
            "\033[31mSyntaxError: unclosed parenthesis \"%s\", add extra \")\"\n\n%s\n\n\033[0m",
            rpar.token,
            getPos(S,rpar)
        );

        printf(
            "\033[31mexpected token \"%s\", got \"%s\"\n\n%s\n\n\033[0m",
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
    cparam->parameters = (Node**) malloc(sizeof(Node*));

    Node *param = expression(S);
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
                "\033[31mSyntaxError: remove extra \"%s\"\n\n%s\n\n\033[0m",
                comma.token,
                getPos(S,comma)
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

    if (match(S,"^"))
    {
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
                    "\033[31mSyntaxError: expects \"expression\" after \"%s\", got \"%s\"\n\n%s\n\n\033[0m",
                    newn->expn->op.token,
                    S->parser.ctok.token,
                    getPos(S,S->parser.ctok)
                );
                exit(1);
            }

            newn->expn->rhs = rhs;
            node = newn;
        }
    }

    return node;
}

Node *factor(State *S)
{
    Token tok = S->parser.ctok;

    if (match(S,"+") || match(S,"-"))
    {
        expect(S,tok.token);

        Node *node  = (Node*) malloc(sizeof(Node));
        node->type  = NUNA;
        node->unary = (Unary_node*) malloc(sizeof(Unary_node));
        node->unary->op = tok;

        Node *expr = factor(S);

        if (expr == NULL)
        {
            printf(
                "\033[31mSyntaxError: expects \"expression\" after \"%s\", got \"%s\"\n\n%s\n\n\033[0m",
                tok.token,
                S->parser.ctok.token,
                getPos(S,S->parser.ctok)
            );

            printf(
                "\033[31minvalid expression \"%s\"\n\n%s\n\n\033[0m",
                S->parser.ctok.token,
                getPos(S,S->parser.ctok)
            );
            exit(1);
        }
        
        node->unary->expr = factor(S);
        return node;
    }
    else if (match(S,"("))
    {
        expect(S,"(");

        Node *expr = expression(S);

        if (expr == NULL)
        {
            printf(
                "\033[31mSyntaxError: expects \"expression\", after \"%s\"\n\n%s\n\n\033[0m",
                tok.token,
                getPos(S,tok)
            );
            printf(
                "\033[31minvalid expression \"%s\"\n\n%s\n\n\033[0m",
                S->parser.ctok.token,
                getPos(S,S->parser.ctok)
            );
            
            exit(1);
        }

        if (!match(S,")"))
        {
            printf(
                "\033[31mSyntaxError: unclosed parenthesis \"%s\", add extra \")\"\n\n%s\n\n\033[0m",
                tok.token,
                getPos(S,tok)
            );

            printf(
                "\033[31mexpected token \"%s\", got \"%s\"\n\n%s\n\n\033[0m",
                ")",
                S->parser.ctok.token,
                getPos(S,S->parser.ctok)
            );

            exit(1);
        }

        expect(S,")");

        return expr;
    }

    return exponential(S);
}

Node *multiplicative(State *S)
{
    Node *node = factor(S);
    if (node == NULL) return node;

    
    if (match(S,"*") || match(S,"/"))
    {
        Node *newn = (Node*) malloc(sizeof(Node));
        newn->type = NARI;
        newn->arithmetic = (Arithmetic_node*) malloc(sizeof(Arithmetic_node));

        newn->arithmetic->lhs = node;
        newn->arithmetic->op  = S->parser.ctok;

        while (!parseeof(S) && (match(S,"*") || match(S,"/")))
        {
            if (match(S,"*"))
            {
                nextToken();
            }
            else if (match(S,"/"))
            {
                nextToken();
            }

            Node *rhs = multiplicative(S);
            
            if (rhs == NULL)
            {
                printf(
                    "\033[31mSyntaxError: expects \"expression\" after \"%s\", got \"%s\"\n\n%s\n\n\033[0m",
                    newn->arithmetic->op.token,
                    S->parser.ctok.token,
                    getPos(S,S->parser.ctok)
                );
                exit(1);
            }

            newn->arithmetic->rhs = rhs;
            node = newn;
        }   
    }
    return node;
}

Node *addetive(State *S)
{
    Node *node = multiplicative(S);
    if (node == NULL) return node;

    if (match(S,"+") || match(S,"-"))
    {
        Node *newn = (Node*) malloc(sizeof(Node));
        newn->type = NARI;
        newn->arithmetic = (Arithmetic_node*) malloc(sizeof(Arithmetic_node));

        newn->arithmetic->lhs = node;
        newn->arithmetic->op  = S->parser.ctok;

        while (!parseeof(S) && (match(S,"+") || match(S,"-")))
        {
            if (match(S,"+"))
            {
                nextToken();
            }
            else if (match(S,"-"))
            {
                nextToken();
            }
            
            Node *rhs = addetive(S);
            
            if (rhs == NULL)
            {
                printf(
                    "\033[31mSyntaxError: expects \"expression\" after \"%s\", got \"%s\"\n\n%s\n\n\033[0m",
                    newn->arithmetic->op.token,
                    S->parser.ctok.token,
                    getPos(S,S->parser.ctok)
                );
                exit(1);
            }

            newn->arithmetic->rhs = rhs;
            node = newn;
        }
    }

    return node;
}

Node *expression(State *S)
{
    return addetive(S);
}

Node *null_safe_expr(State *S)
{
    Node *expr = expression(S);
    if (expr == NULL)
    {
        printf(
            "\033[31mSyntaxError: invalid syntax \"%s\"\n\n%s\n\n\033[0m",
            S->parser.ctok.token,
            getPos(S,S->parser.ctok)
        );
        exit(1);
    }

    return expr;
}

int parseeof(State *S)
{
    return (S->parser.ctok.type == END);
}