#include <stdio.h>
#include <stdlib.h>
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
 * soare_parser.c
*/

AST branch(char *_Value, NodeType _Type)
{
    AST _Branch = (AST)malloc(sizeof(struct ast));
    if (_Branch == NULL)
        return NULL;
    _Branch->_Value = strdup(_Value);
    _Branch->_Type = _Type;
    _Branch->_Child = NULL;
    _Branch->_Parent = NULL;
    _Branch->_SiblingL = NULL;
    _Branch->_SiblingR = NULL;
    _Branch->_Array = NULL;
    return _Branch;
}

void branch_free(AST _Ast)
{
    if (_Ast == NULL)
        return;
    branch_free(_Ast->_Child);
    branch_free(_Ast->_SiblingR);
    free(_Ast->_Value);
    free(_Ast);
}

AST branch_push(AST _Parent, AST _Child)
{
    if (_Parent == NULL || _Child == NULL)
        return EQU(NULL, _Parent, _Child);

    _Child->_Parent = NULL;
    _Child->_SiblingR = NULL;
    _Child->_SiblingL = NULL;

    if (_Parent->_Child == NULL)
        _Parent->_Child = _Child;
    else
    {
        AST tmp = _Parent->_Child;
        while (tmp->_SiblingR != NULL)
            tmp = tmp->_SiblingR;
        tmp->_SiblingR = _Child;
        _Child->_SiblingL = tmp;
    }

    _Child->_Parent = _Parent;
    return _Parent;
}

AST branch_join(AST _Dest, AST _Source)
{
    if (_Dest == NULL || _Source == NULL)
        return EQU(NULL, _Dest, _Source);
    AST tmp = _Source->_Child;
    for (AST next = _Source; tmp != NULL; tmp = next)
    {
        next = tmp->_SiblingR;
        _Dest = branch_push(_Dest, tmp);
    }
    branch_free(_Source->_Parent);
    return _Dest;
}

void branch_puts(AST _Ast)
{
    if (_Ast == NULL)
        return;
    branch_puts(_Ast->_Child);
    branch_puts(_Ast->_SiblingR);
    if (_Ast->_Parent == NULL)
        fprintf(stdout,
                "[AST] {0x%x, '%s'}\n",
                _Ast->_Type,
                _Ast->_Value);
    else
        fprintf(stdout,
                "[AST] {0x%x, '%s'}\t{0x%x, '%s'}\n",
                _Ast->_Parent->_Type,
                _Ast->_Parent->_Value,
                _Ast->_Type,
                _Ast->_Value);
}

static void Next(TOKENS *_Tokens)
{
    *_Tokens = (*_Tokens)->_Next;
}

static void *ThrowsParser(ErrorLang _Err, char *_Extra, size_t _Ln, size_t _Col, AST _Root, AST _Other)
{
    ThrowsLang(_Err, _Extra, _Ln, _Col);
    branch_free(_Root);
    branch_free(_Other);
    return NULL;
}

static AST ParseExpr(TOKENS *_Tokens, unsigned char _MathPriority);

static AST ParseArray(AST _Item, TOKENS *_Tokens, unsigned char _UpdateToken)
{
    if (_Item == NULL || (*_Tokens) == NULL)
        return NULL;

    if (_UpdateToken)
        Next(_Tokens);

    if ((*_Tokens)->_Type != TOKEN_LARRAY)
        return _Item;

    Next(_Tokens);
    AST expr = ParseExpr(_Tokens, 0xF);

    if (expr == NULL || (*_Tokens)->_Type != TOKEN_RARRAY)
        return NULL;

    expr->_Parent = _Item;
    _Item->_Array = expr;
    Next(_Tokens);
    return _Item;
}

static AST ParseFunction(TOKENS *_Tokens, unsigned char _Call)
{
    if ((*_Tokens) == NULL || (*_Tokens)->_Type != TOKEN_NAME)
        return NULL;

    char *name = (*_Tokens)->_Value;
    Next(_Tokens);

    if ((*_Tokens)->_Type != TOKEN_LPAREN)
        return branch(name, NODE_GET);

    Next(_Tokens);
    AST function = branch(name, _Call ? NODE_CALL : NODE_FUNCTION);

    while (1)
    {
        if ((*_Tokens)->_Type == TOKEN_RPAREN)
            break;

        if (!_Call)
        {
            if ((*_Tokens)->_Type == TOKEN_NAME)
                branch_push(function, branch((*_Tokens)->_Value, NODE_GET));
            else
                return NULL;

            Next(_Tokens);
            continue;
        }

        AST expr = ParseExpr(_Tokens, 0xF);
        if (expr == NULL)
            break;
        branch_push(function, expr);
    }

    Next(_Tokens);
    return function;
}

static AST ParseValue(TOKENS *_Tokens)
{
    if ((*_Tokens) == NULL)
        return NULL;

    else if ((*_Tokens)->_Type == TOKEN_STRING || (*_Tokens)->_Type == TOKEN_NUMBER)
        return ParseArray(branch((*_Tokens)->_Value, (*_Tokens)->_Type == TOKEN_STRING ? NODE_STRING : NODE_NUMBER), _Tokens, 0x1);

    else if ((*_Tokens)->_Type == TOKEN_NAME)
        return ParseArray(ParseFunction(_Tokens, 0x1), _Tokens, 0x0);

    return NULL;
}

static AST ParseExpr(TOKENS *_Tokens, unsigned char _MathPriority)
{
    AST left = ParseValue(_Tokens);

    if (left == NULL)
        return NULL;

    while ((*_Tokens)->_Type == TOKEN_OPERATOR)
    {
        unsigned char priority = char_mathPriority(*(*_Tokens)->_Value);

        if (priority >= _MathPriority)
            break;

        AST op = branch((*_Tokens)->_Value, NODE_OPERATOR);
        Next(_Tokens);
        AST right = ParseExpr(_Tokens, priority);

        if (right == NULL || left == NULL || op == NULL)
        {
            branch_free(right);
            branch_free(left);
            branch_free(op);
            return NULL;
        }

        branch_push(op, left);
        branch_push(op, right);
        left = op;
    }

    return left;
}

AST Parser(TOKENS _Tokens)
{
    AST root = branch("ROOT", NODE_ROOT);
    AST CurrentAst = root;
    TOKENS CurrentTokens = _Tokens;

    while (CurrentTokens != NULL)
    {
        TOKENS prev = CurrentTokens;
        Next(&CurrentTokens);

        if (prev->_Type == TOKEN_EOF)
            break;

        else if (prev->_Type == TOKEN_DELIMITER)
            continue;

        if (prev->_Type == TOKEN_KEYWORD)
        {
            char *keyword = prev->_Value;

            if (!strcasecmp(keyword, "PUTS") || !strcasecmp(keyword, "RETURN") || !strcasecmp(keyword, "LOADIMPORT"))
            {
                AST expr = ParseExpr(&CurrentTokens, 0xF);

                if (expr == NULL)
                    CurrentTokens = prev->_Next;

                AST data = branch(keyword, !strcasecmp(keyword, "RETURN") ? NODE_RETURN : (!strcasecmp(keyword, "PUTS") ? NODE_OUT : NODE_IMPORT));

                branch_push(data, expr);
                branch_push(CurrentAst, data);
            }

            else if (!strcasecmp(keyword, "PROMPT"))
            {
                if (CurrentTokens->_Type != TOKEN_NAME)
                    return ThrowsParser(ERROR_SYNTAX, keyword, prev->_Ln, prev->_Col, root, NULL);
                branch_push(CurrentAst, branch(CurrentTokens->_Value, NODE_PROMPT));
                Next(&CurrentTokens);
            }

            else if (!strcasecmp(keyword, "FUNCTION"))
            {
                AST function = ParseFunction(&CurrentTokens, 0x0);

                if (function == NULL)
                    return ThrowsParser(ERROR_SYNTAX, keyword, prev->_Ln, prev->_Col, root, NULL);

                AST BODY = branch("BODY", NODE_BODY);
                branch_push(function, BODY);
                branch_push(CurrentAst, function);

                if (CurrentTokens->_Type == TOKEN_OPEN)
                {
                    CurrentAst = BODY;
                    Next(&CurrentTokens);
                }
            }

            else if (!strcasecmp(keyword, "GLOBAL"))
            {
                AST global = branch(keyword, NODE_GLOBAL);

                for (; CurrentTokens->_Type == TOKEN_NAME; CurrentTokens = CurrentTokens->_Next)
                    branch_push(global, branch(CurrentTokens->_Value, NODE_GET));

                branch_push(CurrentAst, global);
                Next(&CurrentTokens);
            }

            else if (!strcasecmp(keyword, "TO"))
            {
                if (CurrentTokens->_Type != TOKEN_NAME)
                    return ThrowsParser(ERROR_SYNTAX, keyword, prev->_Ln, prev->_Col, root, NULL);
                branch_push(CurrentAst, branch(CurrentTokens->_Value, NODE_GOTO));
                Next(&CurrentTokens);
            }

            else if (!strcasecmp(keyword, "IF") || !strcasecmp(keyword, "WHILE"))
            {
                AST expr = ParseExpr(&CurrentTokens, 0xF);

                if (expr == NULL)
                    return ThrowsParser(ERROR_SYNTAX, keyword, prev->_Ln, prev->_Col, root, NULL);

                AST NODE = branch(keyword, !strcasecmp(keyword, "IF") ? NODE_IF : NODE_WHILE);
                AST BODY = branch("BODY", NODE_BODY);

                branch_push(NODE, expr);
                branch_push(NODE, BODY);
                branch_push(CurrentAst, NODE);

                if (CurrentTokens->_Type == TOKEN_OPEN)
                {
                    CurrentAst = BODY;
                    Next(&CurrentTokens);
                }
            }

            else if (!strcasecmp(keyword, "ELSE"))
            {
                if (CurrentAst->_Parent->_Type != NODE_IF)
                    return ThrowsParser(ERROR_TOKEN, keyword, prev->_Ln, prev->_Col, root, NULL);

                AST expr = ParseExpr(&CurrentTokens, 0xF);

                if (expr == NULL)
                {
                    expr = branch("1", NODE_NUMBER);
                    CurrentTokens = prev->_Next;
                }

                AST BODY = branch("BODY", NODE_BODY);
                branch_push(CurrentAst->_Parent, expr);
                branch_push(CurrentAst->_Parent, BODY);
                CurrentAst = BODY;
            }
        }

        else if (prev->_Type == TOKEN_VARIABLE)
        {
            if (CurrentTokens->_Type != TOKEN_NAME)
                return ThrowsParser(ERROR_SYNTAX, prev->_Value, prev->_Ln, prev->_Col, root, NULL);

            AST var = ParseArray(branch(CurrentTokens->_Value, NODE_SET), &CurrentTokens, 0x1);
            AST expr = ParseExpr(&CurrentTokens, 0xF);

            if (expr == NULL)
                expr = branch(SOARE_NULL, NODE_NUMBER);

            branch_push(var, expr);
            branch_push(CurrentAst, var);
        }

        else if (prev->_Type == TOKEN_SECTION)
        {
            if (CurrentTokens->_Type != TOKEN_NAME)
                return ThrowsParser(ERROR_SYNTAX, prev->_Value, prev->_Ln, prev->_Col, root, NULL);

            branch_push(CurrentAst, branch(CurrentTokens->_Value, NODE_SECTION));
            Next(&CurrentTokens);
        }

        else if (prev->_Type == TOKEN_CLOSE)
        {
            if (CurrentAst->_Parent == NULL || CurrentAst->_Parent->_Parent == NULL)
                return ThrowsParser(ERROR_TOKEN, prev->_Value, prev->_Ln, prev->_Col, root, NULL);
            CurrentAst = CurrentAst->_Parent->_Parent;
        }

        else if (prev->_Type == TOKEN_NAME)
        {
            TOKENS old = prev;
            AST extra = ParseFunction(&prev, 0x1);

            if (extra == NULL || extra->_Type == NODE_GET)
                return ThrowsParser(ERROR_TOKEN, old->_Value, old->_Ln, old->_Col, root, extra);

            branch_push(CurrentAst, extra);
            CurrentTokens = prev;
            Next(&CurrentTokens);
        }

        else
            return ThrowsParser(ERROR_TOKEN, prev->_Value, prev->_Ln, prev->_Col, root, NULL);
    }

    return root;
}
