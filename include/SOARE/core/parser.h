#ifndef __SOARE_PARSER_H__
#define __SOARE_PARSER_H__ 0x1

/* #pragma once */

/**
 *  _____  _____  ___  ______ _____
 * /  ___||  _  |/ _ \ | ___ \  ___|
 * \ `--. | | | / /_\ \| |_/ / |__
 *  `--. \| | | |  _  ||    /|  __|
 * /\__/ /\ \_/ / | | || |\ \| |___
 * \____/  \___/\_| |_/\_| \_\____/
 *
 * Antoine LANDRIEUX (MIT License) <parser.h>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

/**
 * @brief List the different types of nodes
 * @author Antoine LANDRIEUX
 */
typedef enum node_type
{

    NODE_ROOT,
    NODE_TRY,
    NODE_BODY,
    NODE_RAISE,
    NODE_ARRAY,
    NODE_OUTPUT,
    NODE_NUMBER,
    NODE_STRING,
    NODE_IMPORT,
    NODE_CALL,
    NODE_INPUT,
    NODE_FUNCTION,
    NODE_MEMNEW,
    NODE_MEMGET,
    NODE_MEMSET,
    NODE_ENUMERATE,
    NODE_IFERROR,
    NODE_OPERATOR,
    NODE_CONDITION,
    NODE_REPETITION,
    NODE_RETURN

} node_type;

/**
 * @brief Structure of a node
 * @author Antoine LANDRIEUX
 */
typedef struct node
{

    // Value
    char *value;
    // Type
    node_type type;

    // Document
    Document file;

    // Node Parent
    struct node *parent;
    // Node Child
    struct node *child;
    // Node Sibling
    struct node *sibling;

} Node, *AST;

/**
 * @brief Create a new node
 * @author Antoine LANDRIEUX
 *
 * @param value
 * @param type
 * @param file
 * @return Node*
 */
Node *Branch(char *value, node_type type, Document file);

/**
 * @brief Find a node
 * @author Antoine LANDRIEUX
 *
 * @param source
 * @param value
 * @param type
 * @return Node*
 */
Node *BranchFind(AST __restrict__ source, char *__restrict__ value, node_type type);

/**
 * @brief Add a sibling branch
 * @author Antoine LANDRIEUX
 *
 * @param source
 * @param element
 * @return AST
 */
AST BranchJuxtapose(Node *__restrict__ source, AST __restrict__ element);

/**
 * @brief Join 2 branches
 * @author Antoine LANDRIEUX
 *
 * @param parent
 * @param child
 * @return AST
 */
AST BranchJoin(Node *__restrict__ parent, Node *__restrict__ child);

/**
 * @brief Frees the memory allocated by a tree
 * @author Antoine LANDRIEUX
 *
 * @param tree
 */
void TreeFree(AST tree);

/**
 * @brief Display a tree
 * @author Antoine LANDRIEUX
 *
 * @param tree
 */
void TreeLog(AST tree);

/**
 * @brief Turns a sequence of tokens into a tree (AST)
 * @author Antoine LANDRIEUX
 *
 * @param tokens
 * @return AST
 */
AST Parse(Tokens *tokens);

#endif /* __SOARE_PARSER_H__ */