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
 * @brief Create a new node
 * @author Antoine LANDRIEUX
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
 * @brief Find a node
 * @author Antoine LANDRIEUX
 *
 * @param source
 * @param value
 * @param type
 * @return Node*
 */
Node *BranchFind(AST __restrict__ source, char *__restrict__ value, node_type type)
{
    if (!source)
        return NULL;
    if (!strcmp(value, !source->value ? "" : source->value) && source->type == type)
        return source;
    Node *R = BranchFind(source->sibling, value, type);
    Node *L = BranchFind(source->parent ? (source->parent->parent ? source->parent->parent->child : source->parent) : NULL, value, type);
    return R ? R : L;
}

/**
 * @brief Add a sibling branch
 * @author Antoine LANDRIEUX
 *
 * @param source
 * @param element
 * @return AST
 */
AST BranchJuxtapose(Node *__restrict__ source, AST __restrict__ element)
{
    if (!source || !element)
        return source;

    AST tmp = source->sibling;
    source->sibling = element;

    for (; element->sibling; element = element->sibling)
        element->parent = source->parent;

    element->parent = source->parent;
    element->sibling = tmp;

    return source;
}

/**
 * @brief Join 2 branches
 * @author Antoine LANDRIEUX
 *
 * @param parent
 * @param child
 * @return AST
 */
AST BranchJoin(Node * __restrict__ parent, Node * __restrict__ child)
{
    if (!child || !parent)
        return NULL;
    if (!parent->child)
        parent->child = child;
    else
    {
        Node *tmp = parent->child;
        while (tmp->sibling)
            tmp = tmp->sibling;
        tmp->sibling = child;
    }
    child->parent = parent;
    return parent;
}

/**
 * @brief Frees the memory allocated by a tree
 * @author Antoine LANDRIEUX
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

/**
 * @brief Display a tree
 * @author Antoine LANDRIEUX
 *
 * @param tree
 */
void TreeLog(AST tree)
{
    if (!tree)
        return;

    printf("[BRANCH] ");

    if (tree->parent)
        printf(
            "[%s:%.5lld:%.5lld, %.2X, \"%s\"]\t",
            tree->parent->file.file,
            tree->parent->file.ln,
            tree->parent->file.col,
            tree->parent->type,
            tree->parent->value);
    printf(
        "[%s:%.5lld:%.5lld, %.2X, \"%s\"]\n",
        tree->file.file,
        tree->file.ln,
        tree->file.col,
        tree->type,
        tree->value);
    TreeLog(tree->child);
    TreeLog(tree->sibling);
}

/**
 * @brief Turns a sequence of tokens into a tree (AST)
 * @author Antoine LANDRIEUX
 *
 * @param tokens
 * @return AST
 */
AST Parse(Tokens *tokens)
{
    Node *root = Branch("root", NODE_ROOT, EmptyDocument());
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

            if (*(old->value) == '@')
            {
                if (!TokensFollowPattern(tokens, 2, TKN_NAME, TKN_PARENL))
                {
                    TreeFree(root);
                    return LeaveException(SyntaxError, old->value, old->file);
                }

                AST function = Branch(tokens->value, NODE_FUNCTION, old->file);
                AST body = Branch("BODY", NODE_BODY, old->file);
                TokenNext(&tokens, 2);
                BranchJoin(curr, function);

                while (1)
                {
                    if (tokens->type == TKN_PARENR)
                        break;

                    if (tokens->type != TKN_NAME)
                    {
                        TreeFree(root);
                        TreeFree(body);
                        return LeaveException(SyntaxError, tokens->value, tokens->file);
                    }

                    BranchJoin(function, Branch(tokens->value, NODE_MEMSET, tokens->file));
                    TokenNext(&tokens, 1 + (tokens->next->type == TKN_SEMICOLON));
                }

                BranchJoin(function, body);
                tokens = tokens->next;
                curr = body;
            }

            else if (*(old->value) == '$')
            {
                if (!TokensFollowPattern(tokens, 2, TKN_NAME, TKN_ASSIGN))
                {
                    TreeFree(root);
                    return LeaveException(SyntaxError, old->value, old->file);
                }

                old = tokens;
                TokenNext(&tokens, 2);
                AST content = ParseExpr(&tokens, 0xF);

                if (!content)
                {
                    TreeFree(root);
                    return LeaveException(SyntaxError, old->value, old->file);
                }

                BranchJoin(curr, BranchJoin(Branch(old->value, NODE_MEMNEW, old->file), content));
            }

            else if (!strcmp(old->value, KEYWORD_RETURN))
                BranchJoin(curr, BranchJoin(Branch(old->value, NODE_RETURN, old->file), ParseExpr(&tokens, 0xF)));

            else if (!strcmp(old->value, KEYWORD_INPUTCH))
            {
                if (tokens->type != TKN_NAME)
                {
                    TreeFree(root);
                    return LeaveException(SyntaxError, old->value, old->file);
                }

                BranchJoin(curr, Branch(tokens->value, NODE_INPUT, old->file));
                tokens = tokens->next;
            }

            else if (!strcmp(old->value, KEYWORD_LOADIMPORT) || !strcmp(old->value, KEYWORD_RAISE))
            {
                if (tokens->type != TKN_STRING)
                {
                    TreeFree(root);
                    return LeaveException(SyntaxError, old->value, old->file);
                }

                BranchJoin(curr, Branch(tokens->value, strcmp(old->value, KEYWORD_RAISE) ? NODE_IMPORT : NODE_RAISE, old->file));
                tokens = tokens->next;
            }

            else if (!strcmp(old->value, KEYWORD_TRY))
            {
                Node *try = Branch(old->value, NODE_TRY, old->file);
                BranchJoin(try, Branch("BODY", NODE_BODY, old->file));
                BranchJoin(curr, try);

                curr = try->child;
            }

            else if (!strcmp(old->value, KEYWORD_ENUMERATE))
            {
                Node *enumerate = Branch(old->value, NODE_ENUMERATE, old->file);

                for (; tokens->type != TKN_KEYWORD && strcmp(KEYWORD_END, tokens->value); tokens = tokens->next)
                {
                    if (tokens->type != TKN_NAME)
                    {
                        TreeFree(root);
                        TreeFree(enumerate);
                        return LeaveException(SyntaxError, old->value, old->file);
                    }
                    BranchJoin(enumerate, Branch(tokens->value, NODE_MEMSET, tokens->file));
                }

                BranchJoin(curr, enumerate);
                tokens = tokens->next;
            }

            else if (!strcmp(old->value, KEYWORD_IFERROR))
            {
                if (curr == root || curr->parent->type != NODE_TRY || curr->type == NODE_IFERROR)
                {
                    TreeFree(root);
                    return LeaveException(UnexpectedNear, old->value, old->file);
                }

                Node *iferror = Branch(old->value, NODE_IFERROR, old->file);
                BranchJoin(curr->parent, iferror);
                curr = iferror;
            }

            else if (!strcmp(old->value, KEYWORD_IF) || !strcmp(old->value, KEYWORD_WHILE))
            {
                AST content = ParseExpr(&tokens, 0xF);

                if (!content || tokens->type != TKN_KEYWORD || strcmp(tokens->value, "do"))
                {
                    TreeFree(root);
                    return LeaveException(SyntaxError, old->value, old->file);
                }

                AST statement = Branch(old->value, strcmp(old->value, KEYWORD_IF) ? NODE_REPETITION : NODE_CONDITION, old->file);
                AST body = Branch("BODY", NODE_BODY, old->file);

                BranchJoin(statement, content);
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

                AST content = ParseExpr(&tokens, 0xF);

                if (!content || tokens->type != TKN_KEYWORD || strcmp(tokens->value, "do"))
                {
                    TreeFree(root);
                    return LeaveException(SyntaxError, old->value, old->file);
                }

                AST body = Branch("BODY", NODE_BODY, old->file);

                BranchJoin(curr->parent, content);
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

                AST body = Branch("BODY", NODE_BODY, old->file);

                BranchJoin(curr->parent, Branch("1", NODE_NUMBER, old->file));
                BranchJoin(curr->parent, body);

                curr = body;
            }

            else if (!strcmp(old->value, KEYWORD_WRITE))
            {
                AST content = ParseExpr(&tokens, 0xF);

                if (!content)
                {
                    TreeFree(root);
                    return LeaveException(SyntaxError, old->value, old->file);
                }

                BranchJoin(curr, BranchJoin(Branch(old->value, NODE_OUTPUT, old->file), content));
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

            break;

        case TKN_NAME:

            tokens = old;
            AST parsed = ParseValue(&tokens);

            if (!parsed)
            {
                TreeFree(root);
                return LeaveException(SyntaxError, old->value, old->file);
            }

            if (parsed->type == NODE_MEMGET)
            {
                if (tokens->type != TKN_ASSIGN)
                {
                    TreeFree(root);
                    TreeFree(parsed);
                    return LeaveException(UnexpectedNear, old->value, old->file);
                }
                tokens = tokens->next;
                AST content = ParseExpr(&tokens, 0xF);
                if (!content)
                {
                    TreeFree(root);
                    TreeFree(parsed);
                    return LeaveException(SyntaxError, old->value, old->file);
                }
                parsed->type = NODE_MEMSET;
                BranchJoin(curr, BranchJoin(parsed, content));
                break;
            }
            BranchJoin(curr, parsed);
            break;

        default:
            TreeFree(root);
            return LeaveException(UnexpectedNear, old->value, old->file);
        }
    }

    return (AST)root;
}
