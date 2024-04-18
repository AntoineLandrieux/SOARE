#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <SOARE/SOARE.h>

/*
 _____  _____  ___  ______ _____
/  ___||  _  |/ _ \ | ___ \  ___|
\ `--. | | | / /_\ \| |_/ / |__
 `--. \| | | |  _  ||    /|  __|
/\__/ /\ \_/ / | | || |\ \| |___
\____/  \___/\_| |_/\_| \_\____/

 * Antoine LANDRIEUX
 * GNU General Public License v3.0
 * https://www.gnu.org/licenses/
 * 
 * SOARE
 * soare_execute.c
*/

MEMORY variable = NULL;

MEMORY memory(char *_Name, char *_Value, AST _Access)
{
    MEMORY mem = (MEMORY)malloc(sizeof(struct memory));
    if (mem == NULL)
        return NULL;
    mem->_Name = strdup(_Name);
    mem->_Value = _Value;
    mem->_Access = _Access;
    mem->_Next = NULL;
    return mem;
}

MEMORY memory_get(MEMORY _Memory, char *_Name, AST _Access)
{
    MEMORY tmp = _Memory;
    while (tmp != NULL)
        if (!strcmp(tmp->_Name, _Name) && tmp->_Access == _Access)
            return tmp;
        else
            tmp = tmp->_Next;
    return NEQ(_Access, NULL, memory_get(_Memory, _Name, _Access->_Parent));
}

MEMORY memory_push(MEMORY _Dest, MEMORY _Memory)
{
    if (_Dest == NULL || _Memory == NULL)
        return EQU(NULL, _Dest, _Memory);
    MEMORY tmp = _Dest;
    while (tmp->_Next != NULL)
        tmp = tmp->_Next;
    tmp->_Next = _Memory;
    return _Dest;
}

MEMORY memory_set(MEMORY _Memory, char *_Name, char *_Value, AST _Access)
{
    MEMORY get = memory_get(_Memory, _Name, _Access);
    if (get != NULL)
    {
        free(get->_Value);
        get->_Value = _Value;
        return get;
    }
    return memory_push(_Memory, memory(_Name, _Value, _Access));
}

void memory_free(MEMORY _Memory)
{
    if (_Memory == NULL)
        return;
    memory_free(_Memory->_Next);
    free(_Memory->_Name);
    free(_Memory->_Value);
    free(_Memory);
}

void memory_puts(MEMORY _Memory)
{
    if (_Memory == NULL)
        return;
    memory_puts(_Memory->_Next);
    fprintf(stdout,
            "[MEMORY] {'%s', '%s'}\n",
            _Memory->_Name,
            _Memory->_Value);
}

RESULTLANG resultlang(AST _Ast, MEMORY _Memory)
{
    RESULTLANG resultlang = (RESULTLANG)malloc(sizeof(struct resultlang));
    if (resultlang == NULL)
        return NULL;
    resultlang->_Ast = _Ast;
    resultlang->_Memory = _Memory;
    return resultlang;
}

void resultlang_free(RESULTLANG _Resultlang)
{
    if (_Resultlang == NULL)
        return;
    branch_free(_Resultlang->_Ast);
    memory_free(_Resultlang->_Memory);
    free(_Resultlang);
}

RESULTLANG resultlang_puts(RESULTLANG _Resultlang)
{
    if (_Resultlang == NULL)
        return NULL;
    branch_puts(_Resultlang->_Ast);
    memory_puts(_Resultlang->_Memory);
    return _Resultlang;
}

static char *ExecuteFunction(AST _Root, AST _At, char *_Name, AST _Arg);

static char *Array(AST _Root, char *_String, AST _Index)
{
    if (_Index == NULL || _String == NULL)
        return NEQ(_String, NULL, strdup(_String));

    char *expr = EvalAst(_Root, _Index);
    size_t pos = MIN((size_t)strtol(expr, NULL, 10), strlen(_String) - 1);

    char *_Result = string_slice(&_String[pos], 1);
    free(expr);
    free(_String);
    return _Result;
}

char *EvalAst(AST _Root, AST _Ast)
{
    if (_Ast == NULL)
        return strdup(SOARE_NULL);

    switch (_Ast->_Type)
    {
    case NODE_CALL:
        return Array(_Root, ExecuteFunction(_Root, _Ast->_Parent, _Ast->_Value, _Ast->_Child), _Ast->_Array);

    case NODE_STRING:
        return Array(_Root, strdup(_Ast->_Value), _Ast->_Array);

    case NODE_NUMBER:
        return Array(_Root, strdup(_Ast->_Value), _Ast->_Array);

    case NODE_GET:
        MEMORY tmp = memory_get(variable, _Ast->_Value, _Ast->_Parent);
        return Array(_Root, strdup(tmp == NULL ? SOARE_UNDEFINED : tmp->_Value), _Ast->_Array);

    case NODE_OPERATOR:
        return string_eval(EvalAst(_Root, _Ast->_Child), *_Ast->_Value, EvalAst(_Root, _Ast->_Child->_SiblingR));

    default:
        break;
    }
    return strdup(SOARE_NULL);
}

static char *RunInstructions(AST _Root, AST _Current);

static char *ExecuteFunction(AST _Root, AST _At, char *_Name, AST _Arg)
{
    if (_At == NULL || _Root == NULL || _Name == NULL || _At->_Parent == NULL)
        return strdup(SOARE_UNDEFINED);

    AST tmp = _At->_Parent->_Child;

    while (1)
        if (tmp == NULL)
            return ExecuteFunction(_Root, _At->_Parent->_Parent == NULL ? SoareDefineFunction(NULL)->_Child : _At->_Parent, _Name, _Arg);
        else if (!strcmp(tmp->_Value, _Name) && tmp->_Type == NODE_FUNCTION)
            break;
        else
            tmp = tmp->_SiblingR;

    tmp = tmp->_Child;

    for (; tmp->_Type != NODE_BODY; tmp = tmp->_SiblingR)
    {
        if (_Arg == NULL)
            continue;
        memory_set(variable, tmp->_Value, EvalAst(_Root, _Arg), _At->_Parent);
        _Arg = _Arg->_SiblingR;
    }

    char *run = RunInstructions(_Root, tmp->_Child);
    return run == NULL ? strdup(SOARE_NULL) : run;
}

static void ExecuteSection(AST _Root, AST _At, char *_Name)
{
    if (_Root == NULL || _At == NULL || _Name == NULL)
        return;

    if (_At->_Type == NODE_SECTION && !strcmp(_At->_Value, _Name))
        return free(RunInstructions(_Root, _At->_SiblingR));

    ExecuteSection(_Root, _At->_SiblingR, _Name);
    ExecuteSection(_Root, _At->_Child, _Name);
}

static char *RunInstructions(AST _Root, AST _Current)
{
    if (_Root == NULL)
        return NULL;

    while (_Current != NULL)
    {
        char *expr = EvalAst(_Root, _Current->_Child);
        AST tmp = NULL;
        MEMORY get = NULL;

        switch (_Current->_Type)
        {
        case NODE_IMPORT:
            _Root = branch_join(_Root, ExecuteTokens(file_read(expr))->_Ast);
            break;

        case NODE_CALL:
            free(ExecuteFunction(_Root, _Current, _Current->_Value, _Current->_Child));
            break;

        case NODE_GOTO:
            ExecuteSection(_Root, _Root->_Child, _Current->_Value);
            return NULL;

        case NODE_PROMPT:
            free(expr);
            expr = (char *)malloc(SOARE_MAX_INPUT);
            if (expr == NULL || fgets(expr, SOARE_MAX_INPUT, stdin) == NULL)
                break;
            expr[strlen(expr) - 1] = 0;
            memory_set(variable, _Current->_Value, strdup(expr), EQU(_Root, _Current->_Parent, NULL));
            break;

        case NODE_RETURN:
            return expr;

        case NODE_SET:
            if (_Current->_Array == NULL)
            {
                memory_set(variable, _Current->_Value, strdup(expr), EQU(_Root, _Current->_Parent, NULL));
                break;
            }

            get = memory_get(variable, _Current->_Value, _Current->_Parent);
            char _Value = *expr;
            free(expr);

            expr = EvalAst(_Root, _Current->_Array);
            if (get == NULL || string_isNaN(expr))
                break;

            get->_Value[MIN(strlen(get->_Value) - 1, (size_t)atoi(expr))] = _Value;
            break;

        case NODE_GLOBAL:
            MEMORY cur = NULL;
            for (tmp = _Current->_Child; tmp != NULL; tmp = tmp->_SiblingR)
            {
                cur = memory_get(variable, tmp->_Value, tmp->_Parent);
                if (cur != NULL)
                    cur->_Access = NULL;
            }
            break;

        case NODE_OUT:
            fprintf(stdout, "%s\n", expr);
            break;

        case NODE_IF:
            tmp = _Current->_Child;
            while (tmp != NULL)
            {
                if (strcmp(expr, "0"))
                {
                    free(expr);
                    expr = RunInstructions(_Root, tmp->_SiblingR->_Child);
                    if (expr != NULL)
                        return expr;
                    break;
                }
                free(expr);
                tmp = tmp->_SiblingR->_SiblingR;
                expr = EvalAst(_Root, tmp);
            }
            break;

        case NODE_WHILE:
            while (strcmp(expr, "0"))
            {
                free(expr);
                expr = RunInstructions(_Root, _Current->_Child->_SiblingR->_Child);
                if (expr != NULL)
                    return expr;
                free(expr);
                expr = EvalAst(_Root, _Current->_Child);
            }
            break;

        case NODE_CUSTOM:
            RunCustomExecutor(_Current->_Value, expr);
            break;

        default:
            break;
        }
        free(expr);
        _Current = _Current->_SiblingR;
    }

    return NULL;
}

RESULTLANG ExecuteTokens(TOKENS _Tokens)
{
    AST ast = Parser(_Tokens);
    if (_Tokens == NULL || ast == NULL)
        return NULL;
    variable = memory("SOARE", strdup("1"), NULL);
    free(RunInstructions(ast, ast->_Child));
    return resultlang(ast, variable);
}
