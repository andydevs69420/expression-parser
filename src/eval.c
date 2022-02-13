/*
 *   Copyright (c) 2022 
 *   All rights reserved.
 */

#ifndef EVAL_H
#include "eval.h"
#endif

Result eval(State *S,char *expr)
{
    Parser(S,expr);
    Node *ast = parse(S);
    return evaluate(S,ast);
}

Result evaluate(State *S,Node *node)
{
    switch (node->type)
    {
        case NIDN:
            return eval_iden(S,node);
        case NINT:
            return eval_int(S,node);
        case NFLT:
            return eval_flt(S,node);
        case NARI:
            return eval_arithmetic(S,node);

    }
}

Result eval_iden(State *S,Node *node)
{
    // no symbol table
    Result rs;
    char *sym = node->identifier->value.token;

    if (strcmp(sym,"PI") == 0)
    {
        rs.type  = EFLOAT;
        rs.typen = "float";
        rs.value = format("%1.2f",PI);
    }
    else
    {
        printf(
            "\033[31mNameError: \"%s\" is not defined\n\n%s\n\n\033[0m",
            sym,
            getPos(S,node->identifier->value)
        );
        exit(1);
    }
    return rs;
}

Result eval_int(State *S,Node *node)
{
    Result rs;
    rs.type  = EINT;
    rs.typen = "int";
    rs.value = format("%d",atoi(node->integer->value.token));
    return rs;
}

Result eval_flt(State *S,Node *node)
{
    Result rs;
    rs.type  = EFLOAT;
    rs.typen = "float";
    rs.value = format("%lf",atof(node->floatn->value.token));
    return rs;
}

Result eval_arithmetic(State *S,Node *node)
{
    Token op;
    Result rs,lhs,rhs;

    op  = node->arithmetic->op; 
    lhs = evaluate(S,node->arithmetic->lhs);   
    rhs = evaluate(S,node->arithmetic->rhs);   

    if (!(lhs.type == EFLOAT || rhs.type == EFLOAT))
    {
        rs.type  = EINT;
        rs.typen = "int";
    }
    else if(lhs.type == EFLOAT || rhs.type == EFLOAT)
    {
        rs.type  = EFLOAT;
        rs.typen = "float";
    }
    else
    {
        printf(
            "\033[31mTypeError: invalid op \"%s\" for type(s)\n\n%s\n\n\033[0m",
            op.token,
            getPos(S,op)
        );
        exit(1);
    }

    if (strcmp(op.token,"*") == 0)
    {
        switch (rs.type)
        {
            case EINT:
            {
                int a = atoi(lhs.value);
                int b = atoi(rhs.value);
                rs.value = format("%d",(a*b));
                break;
            }
            case EFLOAT:
            {
                if (lhs.type == EINT && rhs.type == EFLOAT)
                {
                    int a    = atoi(lhs.value);
                    float b  = atof(rhs.value);
                    rs.value = format("%.2lf",(a*b));
                }
                else if (lhs.type == EFLOAT && rhs.type == EINT)
                {
                    float a  = atof(lhs.value);
                    int b    = atoi(rhs.value);
                    rs.value = format("%lf",(a*b));
                }
                else
                {
                    float a  = atof(rhs.value);
                    float b  = atof(lhs.value);
                    rs.value = format("%lf",(a*b));
                }
                break;
            }
        }
    }
    else if (strcmp(op.token,"/") == 0)
    {
        switch (rs.type)
        {
            case EINT:
            {
                int a = atoi(lhs.value);
                int b = atoi(rhs.value);
                rs.value = format("%d",(a/b));
                break;
            }
            case EFLOAT:
            {
                if (lhs.type == EINT && rhs.type == EFLOAT)
                {
                    int a    = atoi(lhs.value);
                    float b  = atof(rhs.value);
                    rs.value = format("%.2lf",(a/b));
                }
                else if (lhs.type == EFLOAT && rhs.type == EINT)
                {
                    float a  = atof(lhs.value);
                    int b    = atoi(rhs.value);
                    rs.value = format("%lf",(a/b));
                }
                else
                {
                    float a  = atof(rhs.value);
                    float b  = atof(lhs.value);
                    rs.value = format("%lf",(a/b));
                }
                break;
            }
        }
    }
    else if (strcmp(op.token,"+") == 0)
    {
        switch (rs.type)
        {
            case EINT:
            {
                int a = atoi(lhs.value);
                int b = atoi(rhs.value);
                rs.value = format("%d",(a+b));
                break;
            }
            case EFLOAT:
            {
                if (lhs.type == EINT && rhs.type == EFLOAT)
                {
                    int a    = atoi(lhs.value);
                    float b  = atof(rhs.value);
                    rs.value = format("%.2lf",(a+b));
                }
                else if (lhs.type == EFLOAT && rhs.type == EINT)
                {
                    float a  = atof(lhs.value);
                    int b    = atoi(rhs.value);
                    rs.value = format("%.2lf",(a+b));
                }
                else
                {
                    float a  = atof(rhs.value);
                    float b  = atof(lhs.value);
                    rs.value = format("%.2lf",(a+b));
                }
                break;
            }
        }
    }
    else if (strcmp(op.token,"-") == 0)
    {
        switch (rs.type)
        {
            case EINT:
            {
                int a = atoi(lhs.value);
                int b = atoi(rhs.value);
                rs.value = format("%d",(a-b));
                break;
            }
            case EFLOAT:
            {
                if (lhs.type == EINT && rhs.type == EFLOAT)
                {
                    int a    = atoi(lhs.value);
                    float b  = atof(rhs.value);
                    rs.value = format("%.2lf",(a-b));
                }
                else if (lhs.type == EFLOAT && rhs.type == EINT)
                {
                    float a  = atof(lhs.value);
                    int b    = atoi(rhs.value);
                    rs.value = format("%.2lf",(a-b));
                }
                else
                {
                    float a  = atof(rhs.value);
                    float b  = atof(lhs.value);
                    rs.value = format("%.2lf",(a-b));
                }
                break;
            }
        }
    }
    return rs;
}
