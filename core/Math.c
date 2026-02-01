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
static inline void remove_useless_zeros(char *string)
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
    remove_useless_zeros(str);

    // Duplicate string
    return strdup(str);
}

////////////////////////////////////////////////////////////
static inline char *__boolean(char boolean)
{
    // Convert char:1 to string
    char str[2] = {'0' + !!boolean, 0};
    // Duplicate string
    return strdup(str);
}

////////////////////////////////////////////////////////////
static inline char *__at(document_t file, char *string, long long index)
{
    // Get char from a string at index
    // Size of string
    long long size = (long long)strlen(string);

    // Allow negative index
    if (index < 0)
        index = size + index;
    if (index < 0 || index >= size)
        return soare_leave_exception(IndexOutOfRange, string, file);

    // Duplicate string
    char str[2] = {string[index], 0};
    return strdup(str);
}

////////////////////////////////////////////////////////////
static inline short math_priority(char symbol)
{
    // Looks up the mathematical priority of an operator
    if (symbol == ':')
        return 0;
    if (strchr("/*%^", symbol) != NULL)
        return 1;
    if (strchr("+-,", symbol) != NULL)
        return 2;
    // Minimum priority
    return 3;
}

////////////////////////////////////////////////////////////
static ast_t parse_value(tokens_t **tokens)
{
    if (!tokens)
        return NULL;

    tokens_t *old = *tokens;
    node_t *value = soare_new_node(old->value, NODE_ROOT, old->file);

    if (!old || !value)
        return NULL;

    __tokens_next();

    switch (old->type)
    {
    case TKN_STRING:
    case TKN_NUMBER:
        value->type = NODE_VALUE;
        break;

    case TKN_PARENL:
    {
        value->type = NODE_BODY;

        ast_t expression = soare_parse_expression(tokens, 0xF);

        if (!expression)
        {
            soare_tree_free(value);
            return NULL;
        }

        soare_tree_join(value, expression);

        if (!*tokens || (*tokens)->type != TKN_PARENR)
        {
            soare_tree_free(value);
            return NULL;
        }

        // consume ')'
        __tokens_next();
        break;
    }

    case TKN_NAME:
    {
        value->type = NODE_MEMGET;

        if (*tokens && (*tokens)->type != TKN_PARENL)
            break;

        // If it's a function call, parse the argument list
        value->type = NODE_CALL;
        // consume '('
        __tokens_next();

        while (*tokens && (*tokens)->type != TKN_PARENR)
        {
            ast_t arg = soare_parse_expression(tokens, 0xF);

            if (!arg)
            {
                soare_tree_free(value);
                return NULL;
            }

            soare_tree_join(value, arg);

            if (*tokens && (*tokens)->type == TKN_SEMICOLON)
            {
                // skip separator and continue
                __tokens_next();
                continue;
            }

            break;
        }

        if (!*tokens || (*tokens)->type != TKN_PARENR)
        {
            soare_tree_free(value);
            return NULL;
        }

        // consume ')'
        __tokens_next();
        break;
    }

    default:
        soare_tree_free(value);
        return NULL;
    }

    return value;
}

////////////////////////////////////////////////////////////
ast_t soare_parse_expression(tokens_t **tokens, short priority)
{
    /**
     *
     * Build a math tree
     *
     * Example:
     *
     * tokens: ["4"]->["+"]->["3"]->["*"]->["2.5"]
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

    node_t *x = parse_value(tokens);
    node_t *y = NULL;
    node_t *symbol = NULL;

    if (!x)
        return NULL;

    while ((*tokens)->type == TKN_OPERATOR && !soare_errorlevel())
    {
        short op = math_priority(*(*tokens)->value);

        if (op >= priority)
            break;

        symbol = soare_new_node((*tokens)->value, NODE_OPERATOR, (*tokens)->file);
        (*tokens) = (*tokens)->next;
        y = soare_parse_expression(tokens, op);

        if (!symbol || !y)
        {
            soare_tree_free(x);
            soare_tree_free(y);
            soare_tree_free(symbol);
            return NULL;
        }

        soare_tree_join(symbol, x);
        soare_tree_join(symbol, y);
        x = symbol;
    }

    return x;
}

#include <assert.h>

////////////////////////////////////////////////////////////
char *soare_math(ast_t tree)
{
    if (!tree)
        return NULL;

    switch (tree->type)
    {
    case NODE_BODY:
        return soare_math(tree->child);

    case NODE_CALL:
        return soare_run_function(tree);

    case NODE_VALUE:
    {
        if (tree->value)
            return strdup(tree->value);

        return NULL;
    }

    case NODE_MEMGET:
    {
        soare_variables_t *get = soare_get_variable(tree->value);

        if (!get)
            return soare_leave_exception(UndefinedReference, tree->value, tree->file);

        if (get->body)
            return soare_leave_exception(VariableDefinedAsFunction, tree->value, tree->file);

        if (get->value)
            return strdup(get->value);

        return NULL;
    }

    case NODE_OPERATOR:
    {
        char *sx = soare_math(tree->child);
        char *sy = soare_math(tree->child->sibling);

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

        case '+':
            return __float(dx + dy);

        case '-':
            return __float(dx - dy);

        case '*':
            return __float(dx * dy);

        case '/':
            if (!dy)
                return soare_leave_exception(DivideByZero, tree->value, tree->file);
            return __float(dx / dy);

        case '%':
            if (!dy)
                return soare_leave_exception(DivideByZero, tree->value, tree->file);
            return __int((int)dx % (int)dy);

        case '^':
            return __int((int)dx ^ (int)dy);

        default:
            break;
        }
    }

    default:
        return soare_leave_exception(MathError, tree->value, tree->file);
    }

    return NULL;
}
