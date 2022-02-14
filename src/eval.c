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
        case NCALL:
            return eval_call(S,node);
        case NUNA:
            return eval_unary(S,node);
        case NEXP:
            return eval_exponential(S,node);
        case NARI:
            return eval_arithmetic(S,node);
        default: 
        {
            // debug
            printf(
                "\033[31mun-implemented node \"%d\"\n\033[0m",
                node->type
            );
            exit(1);
        }
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
    else if(strcmp(sym,"log") == 0)
    {
        rs.type  = EFUNC;
        rs.typen = "function";
        rs.value = format("<function %s>",sym);
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

Result eval_call(State *S,Node *node)
{
    // TODO: implement!!
    Result rs,obj;
    obj = evaluate(S,node->call->object);
    Call_param *params = node->call->params;
   
    if (obj.type != EFUNC)
    {
        printf(
            "\033[31mTypeError: \"%s\" is not a function\n\033[0m",
            obj.value
        );
        exit(1);
    }

    if (strcmp(obj.value,"<function log>") == 0)
    {
        if (params->pcount != 1)
        {
            printf(
                "\033[31mTypeError: \"%s\" requires 1 parameter, got %d\n\033[0m",
                obj.value,
                params->pcount
            );
            exit(1);
        }
       
        Result param0 = evaluate(S,params->parameters[0]);
       
        if (param0.type != EFLOAT)
        {
            printf(
                "\033[31mTypeError: requires \"log(x:float)\", but \"log(x:%s)\" given\n\033[0m",
                param0.typen
            );
            exit(1);
        }

        rs.type  = EFLOAT;
        rs.typen = "float";
        rs.value = format("%lf",log(atof(param0.value)));
    }
    else
    {
        // debug
        printf(
            "\033[31mun-implemented function \"%s\"\n\033[0m",
            obj.value
        );
        exit(1);
    }

    return rs;
}

Result eval_unary(State *S,Node *node)
{
    Token op;
    Result rs,expr;

    op   = node->unary->op;
    expr = evaluate(S,node->unary->expr);
    
    rs.type  = expr.type;
    rs.typen = expr.typen;

    if (strcmp(op.token,"+") == 0)
    {
        switch (rs.type)
        {
            case EINT:
            {
                rs.value = format("%d",+atoi(expr.value));
                break;
            }
            case EFLOAT:
            {
                rs.value = format("%lf",+atof(expr.value));
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
                rs.value = format("%d",-atoi(expr.value));
                break;
            }
            case EFLOAT:
            {
                rs.value = format("%lf",-atof(expr.value));
                break;
            }
        }
    }
    return rs;
}

Result eval_exponential(State *S,Node *node)
{
    Token op;
    Result rs,lhs,rhs;

    op  = node->expn->op; 
    lhs = evaluate(S,node->expn->lhs);   
    rhs = evaluate(S,node->expn->rhs);   

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

    if (strcmp(op.token,"^") == 0)
    {
        switch (rs.type)
        {
            case EINT:
            {
                int a = atoi(lhs.value);
                int b = atoi(rhs.value);
                rs.value = format("%.2lf",pow(a,b));
                break;
            }
            case EFLOAT:
            {
                if (lhs.type == EINT && rhs.type == EFLOAT)
                {
                    int a    = atoi(lhs.value);
                    float b  = atof(rhs.value);
                    rs.value = format("%.2lf",pow(a,b));
                }
                else if (lhs.type == EFLOAT && rhs.type == EINT)
                {
                    float a  = atof(lhs.value);
                    int b    = atoi(rhs.value);
                    rs.value = format("%lf",pow(a,b));
                }
                else
                {
                    float a  = atof(rhs.value);
                    float b  = atof(lhs.value);
                    rs.value = format("%lf",pow(a,b));
                }
                break;
            }
        }
    }
    return rs;
}

Result eval_arithmetic(State *S,Node *node)
{
    // FIXME: use strtol,strtod,etc instead!!
    Token op;
    Result rs,lhs,rhs;

    op  = node->arithmetic->op; 
    lhs = evaluate(S,node->arithmetic->lhs);   
    rhs = evaluate(S,node->arithmetic->rhs);   

    if 
    (
        (lhs.type == EINT && rhs.type == EINT)
    )
    {
        rs.type  = EINT;
        rs.typen = "int";
    }
    else if 
    (
        (lhs.type == EFLOAT && rhs.type == EFLOAT) ||
        (lhs.type == EFLOAT && rhs.type == EINT)   ||
        (lhs.type == EINT && rhs.type == EFLOAT)
    )
    {
        rs.type  = EFLOAT;
        rs.typen = "float";
    }
    else
    {
        printf(
            "\033[31mTypeError: invalid op \"%s\" for type(s) %s and %s\n\n%s\n\n\033[0m",
            op.token,
            lhs.typen,
            rhs.typen,
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
                
                if (a == 0 || b == 0)
                {
                    printf(
                        "\033[31mZeroDivisionError: division of \"%d / %d\"\n\n%s\n\n\033[0m",
                        a,
                        b,
                        getPos(S,op)
                    );
                    exit(1);
                }
                rs.value = format("%d",(a/b));
                break;
            }
            case EFLOAT:
            {
                if (lhs.type == EINT && rhs.type == EFLOAT)
                {
                    int a    = atoi(lhs.value);
                    float b  = atof(rhs.value);

                    if (a == 0 || b == 0)
                    {
                        printf(
                            "\033[31mZeroDivisionError: division of \"%d / %lf\"\n\n%s\n\n\033[0m",
                            op.token,
                            getPos(S,op)
                        );
                        exit(1);
                    }

                    rs.value = format("%.2lf",(a/b));
                }
                else if (lhs.type == EFLOAT && rhs.type == EINT)
                {
                    float a  = atof(lhs.value);
                    int b    = atoi(rhs.value);

                    if (a == 0 || b == 0)
                    {
                        printf(
                            "\033[31mZeroDivisionError: division of \"%lf / %d\"\n\n%s\n\n\033[0m",
                            op.token,
                            getPos(S,op)
                        );
                        exit(1);
                    }

                    rs.value = format("%lf",(a/b));
                }
                else
                {
                    float a  = atof(rhs.value);
                    float b  = atof(lhs.value);

                    if (a == 0 || b == 0)
                    {
                        printf(
                            "\033[31mZeroDivisionError: division of \"%lf / %lf\"\n\n%s\n\n\033[0m",
                            op.token,
                            getPos(S,op)
                        );
                        exit(1);
                    }

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

