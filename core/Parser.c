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
 * Antoine LANDRIEUX (MIT License) <Parser.c>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

#include <SOARE/SOARE.h>

/* Global flag to track if all statements are properly closed */
static boolean_t all_statement_closed = bTrue;

/**
 *  Example of AST (Abstract Syntax Tree) :
 *
 *
 *       (node A)
 *        /
 *    (node B)-(node C)-(node D)
 *
 *  ---
 *
 * - parent : Direct parent of the node
 *
 * Example :
 *
 *  (node A) is the parent of (node B)
 *  (node A) is the parent of (node C)
 *  (node A) is the parent of (node D)
 *
 *  ---
 *
 * - child : Direct child of the node
 *
 * Example :
 *
 *  (node B) is the child of (node A)
 *
 *  ---
 *
 *  - sibling : Direct sibling of the node
 *
 * Example :
 *
 *  (node B) is the sibling of (node C)
 *  (node C) is the sibling of (node D)
 *
 */

////////////////////////////////////////////////////////////
boolean_t soare_is_all_statement_closed(void)
{
    return all_statement_closed;
}

////////////////////////////////////////////////////////////
node_t *soare_new_node(char *value, node_type_t type, document_t file)
{
    node_t *node = (node_t *)malloc(sizeof(node_t));

    if (!node)
    {
        SOARE_OUT_OF_MEMORY();
        return NULL;
    }

    node->value = value;

    if (value)
    {
        node->value = strdup(value);
    }

    node->type = type;
    node->file = file;
    node->parent = NULL;
    node->child = NULL;
    node->sibling = NULL;

    return node;
}

////////////////////////////////////////////////////////////
ast_t soare_tree_juxtapose(ast_t source, ast_t element)
{
    if (!source)
    {
        return element;
    }

    if (!element)
    {
        return source;
    }

    /**
     *
     *  source:
     *
     *    (a)
     *    /
     *  (b)-(c)
     *
     * element:
     *
     *    (d)
     *    /
     *  (e)-(f)
     *
     * returns:
     *
     *     (a)-(d)
     *    /     \
     *  (b)-(c) (e)-(f)
     *
     */

    ast_t tmp = source->sibling;
    source->sibling = element;

    for (; element->sibling; element = element->sibling)
    {
        element->parent = source->parent;
    }

    element->parent = source->parent;
    element->sibling = tmp;

    return source;
}

////////////////////////////////////////////////////////////
ast_t soare_tree_join(ast_t parent, ast_t child)
{
    if (!child)
    {
        return parent;
    }

    if (!parent)
    {
        return child;
    }

    /**
     *
     *  parent:
     *
     *    (a)
     *    /
     *  (b)-(c)
     *
     * child:
     *
     *    (d)
     *    /
     *  (e)-(f)
     *
     * returns:
     *
     *     (a)
     *    /
     * (b)-(c)-(d)
     *         /
     *       (e)-(f)
     *
     */

    if (parent->child)
    {
        node_t *tmp = parent->child;

        while (tmp->sibling)
        {
            tmp = tmp->sibling;
        }

        tmp->sibling = child;
    }
    else
    {
        parent->child = child;
    }

    child->parent = parent;
    return parent;
}

////////////////////////////////////////////////////////////
void soare_tree_free(ast_t tree)
{
    if (!tree)
        return;

    soare_tree_free(tree->sibling);
    soare_tree_free(tree->child);
    free(tree->value);
    free(tree);
}

////////////////////////////////////////////////////////////
ast_t soare_parser(tokens_t *tokens)
{
    ast_t root = soare_new_node(NULL, NODE_ROOT, soare_empty_document());
    ast_t curr = root;

    all_statement_closed = bTrue;

    while (tokens)
    {
        if (tokens->type == TKN_EOF)
        {
            break;
        }

        tokens_t *old = tokens;
        tokens = tokens->next;

        switch (old->type)
        {
        case TKN_SEMICOLON:
        {
            break;
        }

        case TKN_KEYWORD:

            if (!strcmp(old->value, KEYWORD_FN))
            {
                if (tokens->type != TKN_NAME || tokens->next->type != TKN_PARENL)
                {
                    soare_tree_free(root);
                    soare_leave_exception(SyntaxError, old->value, old->file);
                    return NULL;
                }

                ast_t function = soare_new_node(tokens->value, NODE_FUNCTION, old->file);
                tokens = tokens->next->next;
                soare_tree_join(curr, function);

                while (1)
                {
                    if (tokens->type == TKN_PARENR)
                    {
                        break;
                    }

                    if (tokens->type != TKN_NAME)
                    {
                        soare_tree_free(root);
                        soare_leave_exception(SyntaxError, old->value, old->file);
                        return NULL;
                    }

                    soare_tree_join(function, soare_new_node(tokens->value, NODE_MEMSET, tokens->file));

                    tokens = tokens->next;

                    if (tokens && tokens->type == TKN_SEMICOLON)
                    {
                        tokens = tokens->next;
                    }
                }

                ast_t body = soare_new_node(NULL, NODE_BODY, old->file);
                soare_tree_join(function, body);
                tokens = tokens->next;
                curr = body;
            }

            else if (!strcmp(old->value, KEYWORD_BREAK))
            {
                soare_tree_join(curr, soare_new_node(NULL, NODE_BREAK, old->file));
            }

            else if (!strcmp(old->value, KEYWORD_LET))
            {
                if (tokens->type != TKN_NAME)
                {
                    soare_tree_free(root);
                    soare_leave_exception(SyntaxError, old->value, old->file);
                    return NULL;
                }

                ast_t let = soare_new_node(tokens->value, NODE_MEMNEW, old->file);

                tokens = tokens->next;

                if (tokens->type != TKN_ASSIGN)
                {
                    soare_tree_join(curr, let);
                    break;
                }

                tokens = tokens->next;

                ast_t content = soare_parse_expression(&tokens, 0xF);

                if (!content)
                {
                    soare_tree_free(let);
                    soare_tree_free(root);
                    soare_leave_exception(ValueError, old->value, old->file);
                    return NULL;
                }

                soare_tree_join(curr, soare_tree_join(let, content));
            }

            else if (!strcmp(old->value, KEYWORD_RETURN))
            {
                ast_t return_stmt = soare_new_node(NULL, NODE_RETURN, old->file);
                ast_t content = soare_parse_expression(&tokens, 0xF);
                soare_tree_join(curr, soare_tree_join(return_stmt, content));
            }

            else if (!strcmp(old->value, KEYWORD_RAISE))
            {
                if (tokens->type != TKN_STRING)
                {
                    soare_tree_free(root);
                    soare_leave_exception(SyntaxError, old->value, old->file);
                    return NULL;
                }

                soare_tree_join(curr, soare_new_node(tokens->value, NODE_RAISE, old->file));
                tokens = tokens->next;
            }

            else if (!strcmp(old->value, KEYWORD_LOADIMPORT))
            {
                if (tokens->type != TKN_STRING)
                {
                    soare_tree_free(root);
                    soare_leave_exception(SyntaxError, old->value, old->file);
                    return NULL;
                }

                soare_tree_join(curr, soare_new_node(tokens->value, NODE_IMPORT, old->file));
                tokens = tokens->next;
            }

            else if (!strcmp(old->value, KEYWORD_TRY))
            {
                ast_t try = soare_new_node(NULL, NODE_TRY, old->file);
                soare_tree_join(try, soare_new_node(NULL, NODE_BODY, old->file));
                soare_tree_join(curr, try);
                curr = try->child;
            }

            else if (!strcmp(old->value, KEYWORD_IFERROR))
            {
                if (curr == root || curr->parent->type != NODE_TRY || curr->type == NODE_IFERROR)
                {
                    soare_tree_free(root);
                    soare_leave_exception(UnexpectedNear, old->value, old->file);
                    return NULL;
                }

                ast_t iferror = soare_new_node(NULL, NODE_IFERROR, old->file);
                soare_tree_join(curr->parent, iferror);
                curr = iferror;

                // iferror as <varname>
                if (!strcmp(tokens->value, KEYWORD_AS) && tokens->type == TKN_KEYWORD)
                {
                    tokens = tokens->next;

                    if (tokens->type != TKN_NAME)
                    {
                        soare_tree_free(root);
                        soare_leave_exception(SyntaxError, old->value, old->file);
                        return NULL;
                    }

                    soare_tree_join(curr, soare_new_node(tokens->value, NODE_STRERROR, old->file));

                    tokens = tokens->next;
                }
            }

            else if (!strcmp(old->value, KEYWORD_IF))
            {
                ast_t condition = soare_parse_expression(&tokens, 0xF);

                if (!condition)
                {
                    soare_tree_free(root);
                    soare_leave_exception(ValueError, old->value, old->file);
                    return NULL;
                }

                ast_t statement = soare_new_node(NULL, NODE_CONDITION, old->file);
                ast_t body = soare_new_node(NULL, NODE_BODY, old->file);

                soare_tree_join(statement, condition);
                soare_tree_join(statement, body);
                soare_tree_join(curr, statement);

                curr = body;
            }

            else if (!strcmp(old->value, KEYWORD_WHILE))
            {
                ast_t condition = soare_parse_expression(&tokens, 0xF);

                if (!condition)
                {
                    soare_tree_free(root);
                    soare_leave_exception(ValueError, old->value, old->file);
                    return NULL;
                }

                ast_t statement = soare_new_node(NULL, NODE_REPETITION, old->file);
                ast_t body = soare_new_node(NULL, NODE_BODY, old->file);

                soare_tree_join(statement, condition);
                soare_tree_join(statement, body);
                soare_tree_join(curr, statement);

                curr = body;
            }

            else if (!strcmp(old->value, KEYWORD_OR))
            {
                if (curr->parent->type != NODE_CONDITION)
                {
                    soare_tree_free(root);
                    soare_leave_exception(UnexpectedNear, old->value, old->file);
                    return NULL;
                }

                ast_t condition = soare_parse_expression(&tokens, 0xF);

                if (!condition)
                {
                    soare_tree_free(root);
                    soare_leave_exception(ValueError, old->value, old->file);
                    return NULL;
                }

                ast_t body = soare_new_node(NULL, NODE_BODY, old->file);

                soare_tree_join(curr->parent, condition);
                soare_tree_join(curr->parent, body);

                curr = body;
            }

            else if (!strcmp(old->value, KEYWORD_ELSE))
            {
                if (curr->parent->type != NODE_CONDITION)
                {
                    soare_tree_free(root);
                    soare_leave_exception(UnexpectedNear, old->value, old->file);
                    return NULL;
                }

                ast_t body = soare_new_node(NULL, NODE_BODY, old->file);

                soare_tree_join(curr->parent, soare_new_node("1", NODE_VALUE, old->file));
                soare_tree_join(curr->parent, body);

                curr = body;
            }

            else if (!strcmp(old->value, KEYWORD_END))
            {
                if (curr == root)
                {
                    soare_tree_free(root);
                    soare_leave_exception(UnexpectedNear, old->value, old->file);
                    return NULL;
                }

                curr = curr->parent->parent;
            }

            else
            {
                // Custom keyword
                soare_tree_join(curr, soare_new_node(old->value, NODE_CUSTOM_KEYWORD, old->file));
            }

            break;

        case TKN_NAME:

            if (tokens->type == TKN_ASSIGN)
            {
                tokens = tokens->next;
                ast_t content = soare_parse_expression(&tokens, 0xF);

                if (!content)
                {
                    soare_tree_free(root);
                    soare_leave_exception(ValueError, old->value, old->file);
                    return NULL;
                }

                ast_t memset = soare_new_node(old->value, NODE_MEMSET, old->file);
                soare_tree_join(memset, content);
                soare_tree_join(curr, memset);
                break;
            }

        default:

            tokens = old;
            ast_t expression = soare_parse_expression(&tokens, 0xF);

            if (!expression)
            {
                soare_tree_free(root);
                soare_leave_exception(UnexpectedNear, old->value, old->file);
                return NULL;
            }

            soare_tree_join(curr, expression);
            break;
        }
    }

    if (soare_errorlevel())
    {
        soare_tree_free(root);
        return NULL;
    }

    all_statement_closed = (boolean_t)(curr == root);
    return root;
}
