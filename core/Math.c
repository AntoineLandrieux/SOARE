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

/**
 * @brief Remove useless zeros
 *
 * @param string
 */
static inline void zeros(char *string)
{
    /**
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
        --end;

    if (*end == '.')
        --end;

    *(end + 1) = 0;
}

/**
 * @brief Convert int to string
 *
 * @param number
 * @return char*
 */
static inline char *__int(int number)
{
    // Convert int to string
    char string[32] = {0};
    sprintf(string, "%d", number);

    // Duplicate string
    return strdup(string);
}

/**
 * @brief Convert float to string
 *
 * @param number
 * @return char*
 */
static inline char *__float(long double number)
{
    // Convert long double to string
    char string[64] = {0};
    sprintf(string, "%Lf", number);

    // Remove useless zeros
    zeros(string);

    // Duplicate string
    return strdup(string);
}

/**
 * @brief Convert boolean to string
 *
 * @param boolean
 * @return char*
 */
static inline char *__boolean(char boolean)
{
    // Convert char:1 to string
    char string[2] = {'0' + (boolean && 1), 0};
    // Duplicate string
    return strdup(string);
}

/**
 * @brief Looks up the mathematical priority of an operator
 *
 * @param symbol
 * @return unsigned char
 */
static unsigned char MathPriority(char symbol)
{
    if (strchr("/*%^", symbol))
        return 1;
    else if (strchr("+-", symbol))
        return 2;
    else if (strchr("<=!>", symbol))
        return 3;
    return 4;
}

/**
 * @brief Return the value as an array
 *
 * @param tokens
 * @return AST
 */
static AST ParseArray(Tokens **tokens)
{
    /**
     *
     * Example:
     *
     * tokens: ["["]->["1"]->["+"]->["2"]->["]"]
     * returns:
     *
     *  (ARRAY)
     *     |
     *    (+)
     *    /
     *  (1)-(2)
     *
     */

    if ((*tokens)->type != TKN_ARRAYL)
        return NULL;

    __tokens_next();
    AST value = ParseExpr(tokens, 0xF);

    if ((*tokens)->type != TKN_ARRAYR)
    {
        TreeFree(value);
        return NULL;
    }

    __tokens_next();
    return BranchJoin(Branch("ARRAY", NODE_ARRAY, (*tokens)->file), value);
}

/**
 * @brief Return the value as a node
 *
 * @param tokens
 * @return AST
 */
AST ParseValue(Tokens **tokens)
{
    Node *value = Branch((*tokens)->value, NODE_ROOT, (*tokens)->file);
    Tokens *old = *tokens;

    __tokens_next();

    switch (old->type)
    {
    case TKN_STRING:
    case TKN_NUMBER:

        value->type = NODE_VALUE;
        break;

    case TKN_NAME:

        value->type = NODE_MEMGET;
        if ((*tokens)->type != TKN_PARENL)
            break;

        value->type = NODE_CALL;
        __tokens_next();
        AST expr = NULL;

        while ((*tokens)->type != TKN_PARENR)
        {
            if (!(expr = ParseExpr(tokens, 0xF)))
            {
                TreeFree(value);
                return NULL;
            }

            BranchJoin(value, expr);

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

    default:

        TreeFree(value);
        return NULL;
    }

    BranchJoin(value, ParseArray(tokens));
    return value;
}

/**
 * @brief Build a math tree
 *
 * @param tokens
 * @param priority
 * @return AST
 */
AST ParseExpr(Tokens **tokens, unsigned char priority)
{

    /**
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

    while ((*tokens)->type == TKN_OPERATOR && !ErrorLevel())
    {
        unsigned char op = MathPriority(*(*tokens)->value);

        if (op >= priority)
            break;

        symbol = Branch((*tokens)->value, NODE_OPERATOR, (*tokens)->file);
        __tokens_next();
        y = ParseExpr(tokens, op);

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

/**
 * @brief Get the Array Index object
 *
 * @param array
 * @param size
 * @return long long
 */
long long GetArrayIndex(AST array, size_t size)
{
    while (array)
        if (array->type != NODE_ARRAY)
            array = array->sibling;
        else
            break;

    if (!array)
        return -1;

    char *index = Eval(array->child);

    if (!index)
        return -1;

    long long indexlld = atoll(index);
    indexlld = indexlld < 0 ? (long long)size + indexlld : indexlld;
    free(index);

    if (size <= (size_t)indexlld || indexlld < 0)
    {
        LeaveException(IndexOutOfRange, array->value, array->file);
        return -1;
    }

    return indexlld;
}

/**
 * @brief Array parser
 *
 * @param value
 * @param array
 * @return char*
 */
static char *Array(char *value, AST array)
{
    if (!value || !array)
        return value;

    long long index = GetArrayIndex(array, strlen(value));
    char *result = NULL;

    if (index < 0)
        return value;

    if (!(result = malloc(2)))
    {
        free(value);
        return __SOARE_OUT_OF_MEMORY();
    }

    0 [result] = value[index];
    1 [result] = 0;

    free(value);
    return result;
}

/**
 * @brief Evaluates the mathematical expression of a tree
 *
 * @param tree
 * @return char*
 */
char *Math(AST tree)
{
    switch (tree->type)
    {
    case NODE_VALUE:

        return strdup(tree->value);

    case NODE_CALL:

        return RunFunction(tree);

    case NODE_MEMGET:
    {
        MEM get = MemGet(MEMORY, tree->value);

        if (!get)
            return LeaveException(UndefinedReference, tree->value, tree->file);

        if (get->body)
            return LeaveException(VariableDefinedAsFunction, tree->value, tree->file);

        return strdup(get->value);
    }

    case NODE_OPERATOR:
    {
        char *sx = Eval(tree->child);
        char *sy = Eval(tree->child->sibling);

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

/**
 * @brief Evaluates the mathematical expression of a tree
 *
 * @param tree
 * @return char*
 */
char *Eval(AST tree)
{
    if (!tree)
        return NULL;

    char *string = Array(Math(tree), tree->child);

    if (ErrorLevel())
    {
        free(string);
        return NULL;
    }

    return string;
}
