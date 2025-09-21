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

/**
 * @brief Create a new node
 *
 * @param value
 * @param type
 * @param file
 * @return Node*
 */
Node *Branch(char *value, node_type type, Document file)
{
    Node *branch = (Node *)malloc(sizeof(Node));

    if (!branch)
        return __SOARE_OUT_OF_MEMORY();

    branch->value = !value ? NULL : strdup(value);
    branch->type = type;
    branch->file = file;
    branch->parent = NULL;
    branch->child = NULL;
    branch->sibling = NULL;

    return branch;
}

/**
 * @brief Add a sibling branch
 *
 * @param source
 * @param element
 * @return AST
 */
AST BranchJuxtapose(Node *source, AST element)
{
    if (!source)
        return element;

    if (!element)
        return source;

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

    AST tmp = source->sibling;
    source->sibling = element;

    for (; element->sibling; element = element->sibling)
    {
        element->parent = source->parent;
    }

    element->parent = source->parent;
    element->sibling = tmp;

    return source;
}

/**
 * @brief Join 2 branches
 *
 * @param parent
 * @param child
 * @return AST
 */
AST BranchJoin(Node *parent, Node *child)
{
    if (!child)
        return parent;

    if (!parent)
        return child;

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
        Node *tmp = parent->child;
        while (tmp->sibling)
            tmp = tmp->sibling;
        tmp->sibling = child;
    }
    else
        parent->child = child;

    child->parent = parent;
    return parent;
}

/**
 * @brief Frees the memory allocated by a tree
 *
 * @param tree
 */
void TreeFree(AST tree)
{
    if (!tree)
        return;

    TreeFree(tree->child);
    TreeFree(tree->sibling);
    free(tree->value);
    free(tree);
}

#ifdef __SOARE_DEBUG

/**
 * @brief Display a tree
 *
 * @param tree
 */
void TreeLog(AST tree)
{
    if (!tree)
        return;

    /**
     *
     * Example:
     *
     * [BRANCH] [(null):00000:00000, 00, "(null)"]
     * [BRANCH] [(null):00000:00000, 00, "(null)"]     [test.soare:00002:00001, 05, "write"]
     * [BRANCH] [test.soare:00002:00001, 05, "write"]  [test.soare:00002:00007, 07, "Hello World!"]
     *
     */

    soare_write(__soare_stdout, "[BRANCH] ");

    if (tree->parent)
        soare_write(
            //
            __soare_stdout,
            "[%s:%.5lld:%.5lld, %.2X, \"%s\"]\t",
            tree->parent->file.file,
            tree->parent->file.ln,
            tree->parent->file.col,
            tree->parent->type,
            tree->parent->value
            //
        );
    soare_write(
        //
        __soare_stdout,
        "[%s:%.5lld:%.5lld, %.2X, \"%s\"]\n",
        tree->file.file,
        tree->file.ln,
        tree->file.col,
        tree->type,
        tree->value
        //
    );
    TreeLog(tree->child);
    TreeLog(tree->sibling);
}

#endif /* __SOARE_DEBUG */

/**
 * @brief Turns a sequence of tokens into a tree (AST)
 *
 * @param tokens
 * @return AST
 */
AST Parse(Tokens *tokens)
{
    Node *root = Branch(NULL, NODE_ROOT, EmptyDocument());
    Node *curr = root;

    while (tokens)
    {
        if (tokens->type == TKN_EOF)
            break;

        Tokens *old = tokens;
        tokens = tokens->next;

        switch (old->type)
        {
        case TKN_SEMICOLON:
            break;

        case TKN_KEYWORD:

            if (!strcmp(old->value, KEYWORD_FN))
            {
                if (!TokensFollowPattern(tokens, 2, TKN_NAME, TKN_PARENL))
                {
                    TreeFree(root);
                    return LeaveException(SyntaxError, old->value, old->file);
                }

                /**
                 *
                 *       \
                 *       (function)
                 *      /
                 *  (arg1)-(arg2)-(body)
                 *                   \...
                 *
                 */

                AST function = Branch(tokens->value, NODE_FUNCTION, old->file);
                tokens = tokens->next->next;
                BranchJoin(curr, function);

                while (1)
                {
                    if (tokens->type == TKN_PARENR)
                        break;

                    if (tokens->type != TKN_NAME)
                    {
                        TreeFree(root);
                        return LeaveException(SyntaxError, old->value, old->file);
                    }

                    BranchJoin(function, Branch(tokens->value, NODE_MEMSET, tokens->file));

                    tokens = tokens->next;

                    if (tokens && tokens->type == TKN_SEMICOLON)
                        tokens = tokens->next;
                }

                AST body = Branch(NULL, NODE_BODY, old->file);
                BranchJoin(function, body);
                tokens = tokens->next;
                curr = body;
            }

            else if (!strcmp(old->value, KEYWORD_BREAK))
            {
                /**
                 *
                 *     \
                 *     (break)
                 *
                 */
                BranchJoin(curr, Branch(NULL, NODE_BREAK, old->file));
            }

            else if (!strcmp(old->value, KEYWORD_LET))
            {
                if (!TokensFollowPattern(tokens, 2, TKN_NAME, TKN_ASSIGN))
                {
                    TreeFree(root);
                    return LeaveException(SyntaxError, old->value, old->file);
                }

                tokens = tokens->next->next;
                AST content = ParseExpr(&tokens, 0xF);

                if (!content)
                {
                    TreeFree(root);
                    return LeaveException(ValueError, old->value, old->file);
                }

                /**
                 *
                 *     \
                 *     (varname)
                 *         |
                 *      (value)
                 */

                BranchJoin(curr, BranchJoin(Branch(old->next->value, NODE_MEMNEW, old->file), content));
            }

            else if (!strcmp(old->value, KEYWORD_RETURN))
            {

                /**
                 *
                 *     \
                 *     (return)
                 *         |
                 *      (value)
                 */

                BranchJoin(curr, BranchJoin(Branch(NULL, NODE_RETURN, old->file), ParseExpr(&tokens, 0xF)));
            }

            else if (!strcmp(old->value, KEYWORD_RAISE) || !strcmp(old->value, KEYWORD_LOADIMPORT))
            {
                if (tokens->type != TKN_STRING)
                {
                    TreeFree(root);
                    return LeaveException(SyntaxError, old->value, old->file);
                }

                /**
                 *
                 *      \
                 *    (raise/loadimport)
                 *           |
                 *        (string)
                 *
                 */

                // Please note that `raise` and `loadimport` have the same structure
                // If you changed KEYWORD_RAISE or KEYWORD_LOADIMPORT, please use :
                // !strcmp(old->value, KEYWORD_RAISE) ? NODE_RAISE : NODE_IMPORT;
                node_type type = *(old->value) == KEYWORD_RAISE[0] ? NODE_RAISE : NODE_IMPORT;
                BranchJoin(curr, Branch(tokens->value, type, old->file));
                tokens = tokens->next;
            }

            else if (!strcmp(old->value, KEYWORD_TRY))
            {
                Node *try = Branch(NULL, NODE_TRY, old->file);
                BranchJoin(try, Branch(NULL, NODE_BODY, old->file));
                BranchJoin(curr, try);

                /**
                 *
                 *      \
                 *     (try)
                 *       |...
                 *
                 */

                curr = try->child;
            }

            else if (!strcmp(old->value, KEYWORD_IFERROR))
            {
                if (curr == root || curr->parent->type != NODE_TRY || curr->type == NODE_IFERROR)
                {
                    TreeFree(root);
                    return LeaveException(UnexpectedNear, old->value, old->file);
                }

                /**
                 *
                 *      /
                 *  (try)-(iferror)
                 *    |...    |...
                 *
                 */

                Node *iferror = Branch(NULL, NODE_IFERROR, old->file);
                BranchJoin(curr->parent, iferror);
                curr = iferror;
            }

            else if (!strcmp(old->value, KEYWORD_IF) || !strcmp(old->value, KEYWORD_WHILE))
            {
                AST condition = ParseExpr(&tokens, 0xF);

                if (!condition)
                {
                    TreeFree(root);
                    return LeaveException(ValueError, old->value, old->file);
                }

                if (tokens->type != TKN_KEYWORD || strcmp(tokens->value, "do"))
                {
                    TreeFree(root);
                    return LeaveException(SyntaxError, old->value, old->file);
                }

                /**
                 *
                 *
                 *      (while/if)
                 *         /
                 *   (condition)-(body)
                 * .../             \...
                 *
                 */

                // Please note that `if` and `while` have the same structure
                // If you changed KEYWORD_WHILE or KEYWORD_IF, please use :
                // !strcmp(old->value, KEYWORD_IF) ? NODE_CONDITION : NODE_REPETITION;
                node_type type = *(old->value) == KEYWORD_IF[0] ? NODE_CONDITION : NODE_REPETITION;
                AST statement = Branch(NULL, type, old->file);
                AST body = Branch(NULL, NODE_BODY, old->file);

                BranchJoin(statement, condition);
                BranchJoin(statement, body);
                BranchJoin(curr, statement);

                curr = body;
                tokens = tokens->next;
            }

            else if (!strcmp(old->value, KEYWORD_OR))
            {
                if (curr->parent->type != NODE_CONDITION)
                {
                    TreeFree(root);
                    return LeaveException(UnexpectedNear, old->value, old->file);
                }

                AST condition = ParseExpr(&tokens, 0xF);

                if (!condition)
                {
                    TreeFree(root);
                    return LeaveException(ValueError, old->value, old->file);
                }

                if (tokens->type != TKN_KEYWORD || strcmp(tokens->value, "do"))
                {
                    TreeFree(root);
                    return LeaveException(SyntaxError, old->value, old->file);
                }

                /**
                 *
                 *                /
                 *             (if)
                 *             /
                 *   (condition 1)-(body 1)-(condition 2)-(body 2)
                 *        |...        |...       |...        |...
                 *
                 */

                AST body = Branch(NULL, NODE_BODY, old->file);

                BranchJoin(curr->parent, condition);
                BranchJoin(curr->parent, body);

                curr = body;
                tokens = tokens->next;
            }

            else if (!strcmp(old->value, KEYWORD_ELSE))
            {
                if (curr->parent->type != NODE_CONDITION)
                {
                    TreeFree(root);
                    return LeaveException(UnexpectedNear, old->value, old->file);
                }

                /**
                 *
                 *                /
                 *             (if)--------(else)
                 *             /            /
                 *   (condition)--(body)  (1)--(body)
                 *                   |...        |...
                 *
                 */

                AST body = Branch(NULL, NODE_BODY, old->file);

                BranchJoin(curr->parent, Branch("1", NODE_VALUE, old->file));
                BranchJoin(curr->parent, body);

                curr = body;
            }

            else if (!strcmp(old->value, KEYWORD_END))
            {
                if (curr == root)
                {
                    TreeFree(root);
                    return LeaveException(UnexpectedNear, old->value, old->file);
                }

                curr = curr->parent->parent;
            }

            else
            {
                // Custom keyword
                BranchJoin(curr, Branch(old->value, NODE_CUSTOM_KEYWORD, old->file));
            }

            break;

        case TKN_NAME:

            if (tokens->type != TKN_PARENL)
            {
                if (tokens->type != TKN_ASSIGN)
                {
                    TreeFree(root);
                    return LeaveException(UnexpectedNear, old->value, old->file);
                }

                tokens = tokens->next;
                AST content = ParseExpr(&tokens, 0xF);

                if (!content)
                {
                    TreeFree(root);
                    return LeaveException(ValueError, old->value, old->file);
                }

                /**
                 *
                 *      \
                 *     (varname)
                 *         |
                 *      (value)
                 *
                 */

                Node *memset = Branch(old->value, NODE_MEMSET, old->file);
                BranchJoin(memset, content);
                BranchJoin(curr, memset);
                break;
            }

            tokens = old;
            AST call = ParseValue(&tokens);

            if (!call)
            {
                TreeFree(root);
                return LeaveException(ValueError, old->value, old->file);
            }

            /**
             *
             *      \
             *    (function name)
             *      /
             *  (arg1)-(arg2)-(arg...)
             *
             */

            BranchJoin(curr, call);
            break;

        default:

            TreeFree(root);
            return LeaveException(UnexpectedNear, old->value, old->file);
        }
    }

    if (ErrorLevel())
    {
        TreeFree(root);
        return NULL;
    }

    return (AST)root;
}
