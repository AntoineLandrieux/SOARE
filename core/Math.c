#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 *  _____  _____  ___  ______ _____
 * /  ___||  _  |/ _ \ | ___ \  ___|
 * \ `--. | | | / /_\ \| |_/ / |__
 *  `--. \| | | |  _  ||    /|  __|
 * /\__/ /\ \_/ / | | || |\ \| |___
 * \____/  \___/\_| |_/\_| \_\____/
 *
 * Antoine LANDRIEUX (MIT License) <Math.c>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

#include <SOARE/SOARE.h>

/* A clean way to write (*tokens) = (*tokens)->next */
#define __tokens_next() (*tokens) = (*tokens)->next

////////////////////////////////////////////////////////////
static inline void zeros(char *string)
{
    /**
     *
     * Remove useless zeros
     *
     * Example:
     *
     * 100      : 100
     * 10.5     : 10.5
     * 9.50     : 9.5
     * 12.      : 12
     * 64.000   : 64
     *
     */

    char *end = string + strlen(string) - 1;

    while (end > string && *end == '0')
        end--;

    if (*end == '.')
        end--;

    *(end + 1) = 0;
}

////////////////////////////////////////////////////////////
static inline char *__int(int number)
{
    // Convert int to string
    char str[32] = {0};
    sprintf(str, "%d", number);

    // Duplicate string
    return strdup(str);
}

////////////////////////////////////////////////////////////
static inline char *__float(long double number)
{
    // Convert long double to string
    char str[64] = {0};
    sprintf(str, "%Lf", number);

    // Remove useless zeros
    zeros(str);

    // Duplicate string
    return strdup(str);
}

////////////////////////////////////////////////////////////
static inline char *__boolean(char boolean)
{
    // Convert char:1 to string
    char str[2] = {'0' + (boolean && 1), 0};
    // Duplicate string
    return strdup(str);
}

////////////////////////////////////////////////////////////
static inline char *__at(Document file, char *string, long long index)
{
    // Get char from a string at index
    // Size of string
    long long size = (long long)strlen(string);

    // Allow negative index
    if (index < 0)
        index = size + index;
    if (index < 0 || index >= size)
        return LeaveException(IndexOutOfRange, string, file);

    // Duplicate string
    char str[2] = {string[index], 0};
    return strdup(str);
}

////////////////////////////////////////////////////////////
static inline unsigned char MathPriority(char symbol)
{
    // Looks up the mathematical priority of an operator
    if (symbol == ':')
        return 0;
    if (strchr("/*%^", symbol))
        return 1;
    if (strchr("+-", symbol))
        return 2;
    return 3;
}

////////////////////////////////////////////////////////////
AST ParseValue(Tokens **tokens)
{
    Node *value = Branch((*tokens)->value, NODE_ROOT, (*tokens)->file);

    if (!value)
        return NULL;

    Tokens *old = *tokens;

    __tokens_next();

    switch (old->type)
    {
    case TKN_STRING:
    case TKN_NUMBER:

        value->type = NODE_VALUE;
        break;

    case TKN_PARENL:

        value->type = NODE_BODY;

        BranchJoin(value, ParseExpression(tokens, 0xF));

        if ((*tokens)->type != TKN_PARENR)
        {
            TreeFree(value);
            return NULL;
        }

        __tokens_next();
        break;

    case TKN_NAME:
    {
        value->type = NODE_MEMGET;

        if ((*tokens)->type != TKN_PARENL)
            break;

        value->type = NODE_CALL;
        AST expression = NULL;

        __tokens_next();

        while ((*tokens)->type != TKN_PARENR)
        {
            if (!(expression = ParseExpression(tokens, 0xF)))
            {
                TreeFree(value);
                return NULL;
            }

            BranchJoin(value, expression);

            if ((*tokens)->type != TKN_SEMICOLON)
                break;

            __tokens_next();
        }

        if ((*tokens)->type != TKN_PARENR)
        {
            TreeFree(value);
            return NULL;
        }

        __tokens_next();
        break;
    }

    default:

        TreeFree(value);
        return NULL;
    }

    return value;
}

////////////////////////////////////////////////////////////
AST ParseExpression(Tokens **tokens, unsigned char priority)
{

    /**
     *
     * Build a math tree
     *
     * Example:
     *
     * Tokens: ["4"]->["+"]->["3"]->["*"]->["2.5"]
     *
     * Result:
     *
     *    (+)
     *    /
     *  (4)-(*)
     *      /
     *    (3)-(2.5)
     *
     */

    Node *x = ParseValue(tokens);
    Node *y = NULL;
    Node *symbol = NULL;

    if (!x)
        return NULL;

    while ((*tokens)->type == TKN_OPERATOR && !soare_errorlevel())
    {
        unsigned char op = MathPriority(*(*tokens)->value);

        if (op >= priority)
            break;

        symbol = Branch((*tokens)->value, NODE_OPERATOR, (*tokens)->file);

        __tokens_next();
        y = ParseExpression(tokens, op);

        if (!symbol || !y)
        {
            TreeFree(x);
            TreeFree(y);
            TreeFree(symbol);
            return NULL;
        }

        BranchJoin(symbol, x);
        BranchJoin(symbol, y);
        x = symbol;
    }

    return x;
}

////////////////////////////////////////////////////////////
char *Math(AST tree)
{
    if (!tree)
        return NULL;

    switch (tree->type)
    {
    case NODE_VALUE:

        return tree->value ? strdup(tree->value) : NULL;

    case NODE_CALL:

        return RunFunction(tree);

    case NODE_BODY:

        return Math(tree->child);

    case NODE_MEMGET:
    {
        MEM get = MemGet(MEMORY, tree->value);

        if (!get)
            return LeaveException(UndefinedReference, tree->value, tree->file);

        if (get->body)
            return LeaveException(VariableDefinedAsFunction, tree->value, tree->file);

        return get->value ? strdup(get->value) : NULL;
    }

    case NODE_OPERATOR:
    {
        char *sx = Math(tree->child);
        char *sy = Math(tree->child->sibling);

        if (!sx || !sy)
        {
            free(sx);
            free(sy);
            return NULL;
        }

        char *result = NULL;

        switch (*(tree->value))
        {
        case ',':
            if (!(result = malloc(strlen(sx) + strlen(sy) + 1)))
                return __SOARE_OUT_OF_MEMORY();

            strcat(strcpy(result, sx), sy);
            free(sx);
            free(sy);
            return result;

        case '=':
            result = __boolean(!strcmp(sx, sy));
            free(sx);
            free(sy);
            return result;

        case '~':
        case '!':
            result = __boolean(strcmp(sx, sy));
            free(sx);
            free(sy);
            return result;

        case ':':
            result = __at(tree->file, sx, strtoll(sy, &result, 10));
            free(sx);
            free(sy);
            return result;

        default:
            break;
        }

        long double dx = strtold(sx, &result);
        long double dy = strtold(sy, &result);

        free(sx);
        free(sy);

        if ((*(tree->value) == '/' || *(tree->value) == '%') && !dy)
            return LeaveException(DivideByZero, tree->value, tree->file);

        switch (*(tree->value))
        {
        // < or <=
        case '<':
            return __boolean(dx < dy || (tree->value[1] == '=' && dx == dy));

        // > or >=
        case '>':
            return __boolean(dx > dy || (tree->value[1] == '=' && dx == dy));

        case '&':
            return __boolean(dx && dy);

        case '|':
            return __boolean(dx || dy);

        case '^':
            return __int((int)dx ^ (int)dy);

        case '%':
            return __int((int)dx % (int)dy);

        case '*':
            return __float(dx * dy);

        case '/':
            return __float(dx / dy);

        case '+':
            return __float(dx + dy);

        case '-':
            return __float(dx - dy);

        default:
            return LeaveException(MathError, tree->value, tree->file);
        }
    }

    default:
        return LeaveException(MathError, tree->value, tree->file);
    }

    return NULL;
}
