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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Return 1 if the string is a number
 * @author Antoine LANDRIEUX
 *
 * @param string
 * @return u8
 */
static u8 isNaN(char *string)
{
    if (!string)
        return 1;
    if (*string == '+' || *string == '-')
        (volatile char *)string++;
    for (u8 dot = 1; *string; (volatile char *)string++)
        if (*string == '.' && dot)
            dot = 0;
        else if (!(*string >= '0' && *string <= '9'))
            return 1;
    return 0;
}

/**
 * @brief Copy a string
 * @author Antoine LANDRIEUX
 *
 * @param value
 * @return char*
 */
static char *vardup(char *value)
{
    char *result = strdup(value);
    if (!result)
        return __SOARE_OUT_OF_MEMORY();
    return result;
}

/**
 * @brief Remove useless zeros
 * @author Antoine LANDRIEUX
 *
 * @param string
 */
static void zeros(char *string)
{
    char *end = string + strlen(string) - 1;
    while (end > string && *end == '0')
        --end;
    if (*end == '.')
        --end;
    *(end + 1) = 0;
}

/**
 * @brief Convert float to string
 * @author Antoine LANDRIEUX
 *
 * @param number
 * @return char*
 */
static char *__float(long double number)
{
    char string[100] = {0};
    sprintf(string, "%Lf", number);
    zeros(string);
    char *result = (char *)malloc(strlen(string) + 1);
    if (!result)
        return __SOARE_OUT_OF_MEMORY();
    strcpy(result, string);
    return result;
}

/**
 * @brief Looks up the mathematical priority of an operator
 * @author Antoine LANDRIEUX
 *
 * @param symbol
 * @return u8
 */
static u8 MathPriority(char symbol)
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
 * @author Antoine LANDRIEUX
 *
 * @param tokens
 * @return AST
 */
static AST ParseArray(Tokens **tokens)
{
    if ((*tokens)->type != TKN_ARRAYL)
        return NULL;
    TokenNext(tokens, 1);
    AST value = ParseExpr(tokens, 0xF);
    if ((*tokens)->type != TKN_ARRAYR)
        return NULL;
    TokenNext(tokens, 1);
    return BranchJoin(Branch("ARRAY", NODE_ARRAY, (*tokens)->file), value);
}

/**
 * @brief Return the value as a node
 * @author Antoine LANDRIEUX
 *
 * @param tokens
 * @return AST
 */
AST ParseValue(Tokens **tokens)
{
    Node *value = Branch((*tokens)->value, NODE_ROOT, (*tokens)->file);
    Tokens *old = *tokens;
    TokenNext(tokens, 1);

    switch (old->type)
    {
    case TKN_NUMBER:
        value->type = NODE_NUMBER;
        break;

    case TKN_STRING:
        value->type = NODE_STRING;
        break;

    case TKN_NAME:
        value->type = NODE_MEMGET;
        if ((*tokens)->type != TKN_PARENL)
            break;

        value->type = NODE_CALL;
        TokenNext(tokens, 1);
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
            TokenNext(tokens, 1);
        }

        if ((*tokens)->type != TKN_PARENR)
        {
            TreeFree(value);
            return NULL;
        }
        TokenNext(tokens, 1);
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
 * @author Antoine LANDRIEUX
 *
 * @param tokens
 * @param priority
 * @return AST
 */
AST ParseExpr(Tokens **tokens, u8 priority)
{
    Node *x = ParseValue(tokens);
    Node *y = NULL;
    Node *symbol = NULL;

    if (!x)
        return NULL;

    while ((*tokens)->type == TKN_OPERATOR && !ErrorLevel())
    {
        u8 op = MathPriority(*(*tokens)->value);

        if (op >= priority)
            break;

        symbol = Branch((*tokens)->value, NODE_OPERATOR, (*tokens)->file);
        TokenNext(tokens, 1);
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
 * @author Antoine LANDRIEUX
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

    array = array->child;
    char *index = Eval(array);

    if (!index)
        return -1;

    long long indexlld = atoll(index);
    free(index);
    indexlld = indexlld < 0 ? (long long)size + indexlld : indexlld;

    if (size <= (size_t)indexlld || indexlld < 0)
    {
        LeaveException(IndexOutOfRange, array->value, array->file);
        return -1;
    }

    return indexlld;
}

/**
 * @brief Array parser
 * @author Antoine LANDRIEUX
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

    if (index < 0)
        return value;

    char *result = malloc(2);

    if (!result)
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
 * @author Antoine LANDRIEUX
 *
 * @param tree
 * @return char*
 */
char *Math(AST tree)
{
    long double dx, dy;
    char *sx, *sy;
    char *result = NULL;

    MEM get = NULL;

    switch (tree->type)
    {
    case NODE_MEMGET:

        get = MemGet(MEMORY, tree->value);
        if (!get)
            return LeaveException(UndefinedReference, tree->value, tree->file);
        return vardup(get->value);

    case NODE_CALL:

        return RunFunction(tree);

    case NODE_STRING:
    case NODE_NUMBER:

        return vardup(tree->value);

    case NODE_OPERATOR:

        sx = Eval(tree->child);
        sy = Eval(tree->child->sibling);

        if (!sx || !sy)
            return NULL;

        if (*(tree->value) == ',')
        {
            if (!(result = malloc(strlen(sx) + strlen(sy) + 1)))
                return __SOARE_OUT_OF_MEMORY();
            strcat(strcpy(result, sx), sy);
            return result;
        }

        if (isNaN(sx) || isNaN(sy))
        {
            if (!(result = malloc(2)))
                return __SOARE_OUT_OF_MEMORY();

            switch (*(tree->value))
            {
            case '&':
                snprintf(result, 2, "%d", *sx && *sy);
                break;
            case '=':
                snprintf(result, 2, "%d", !strcmp(sx, sy));
                break;
            case '!':
                snprintf(result, 2, "%d", strcmp(sx, sy));
                break;
            case '|':
                snprintf(result, 2, "%d", *sx || *sy);
                break;
            default:
                free(result);
                return LeaveException(MathError, tree->value, tree->file);
            }
            return result;
        }

        dx = strtold(sx, &result);
        dy = strtold(sy, &result);

        if (strchr("/%", *(tree->value)) && !dy)
            return LeaveException(DivideByZero, tree->value, tree->file);

        switch (*(tree->value))
        {
        case '&':
            return __float(dx && dy);
        case '=':
            return __float(dx == dy);
        case '!':
            return __float(dx != dy);
        case '|':
            return __float(dx || dy);
        case '^':
            return __float((long double)((int)dx ^ (int)dy));
        case '%':
            return __float((long double)((int)dx % (int)dy));
        case '*':
            return __float(dx * dy);
        case '/':
            return __float(dx / dy);
        case '+':
            return __float(dx + dy);
        case '-':
            return __float(dx - dy);
        case '<':
            return __float(dx < dy || (dx == dy && tree->value[1] == '='));
        case '>':
            return __float(dx > dy || (dx == dy && tree->value[1] == '='));
        default:
            return LeaveException(MathError, tree->value, tree->file);
        }

    default:
        return LeaveException(MathError, tree->value, tree->file);
    }

    return NULL;
}

/**
 * @brief Evaluates the mathematical expression of a tree
 * @author Antoine LANDRIEUX
 *
 * @param tree
 * @return char*
 */
char *Eval(AST tree)
{
    char *string = NULL;
    if (tree)
    {
        string = Array(Math(tree), tree->child);
        if (ErrorLevel())
        {
            free(string);
            return NULL;
        }
    }
    return string;
}
