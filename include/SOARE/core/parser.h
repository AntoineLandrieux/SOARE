#ifndef __SOARE_PARSER_H__
#define __SOARE_PARSER_H__

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
 * @enum node_type
 * @brief Different kinds of AST nodes produced by the parser
 */
typedef enum node_type
{

    NODE_ROOT,          /**< Root of the document */
    NODE_BODY,          /**< Sequence of statements */
    NODE_TRY,           /**< Try block */
    NODE_RAISE,         /**< Raise/throw statement */
    NODE_VALUE,         /**< Literal or value node */
    NODE_IMPORT,        /**< Import statement */
    NODE_CALL,          /**< Function call */
    NODE_TYPE,          /**< Type annotation or cast */
    NODE_FUNCTION,      /**< Function definition */
    NODE_MEMNEW,        /**< Memory allocation / variable declaration */
    NODE_MEMGET,        /**< Memory read */
    NODE_MEMSET,        /**< Memory write */
    NODE_IFERROR,       /**< If-error construct */
    NODE_OPERATOR,      /**< Operator node (+, -, *, /, etc.) */
    NODE_CONDITION,     /**< Conditional expression */
    NODE_REPETITION,    /**< Loop/repetition construct */
    NODE_BREAK,         /**< Break statement */
    NODE_RETURN,        /**< Return statement */
    NODE_STRERROR,      /**< String error node */
    NODE_CUSTOM_KEYWORD /**< Custom keyword handled by the runtime */

} node_type_t;

/**
 * @struct node
 * @brief Represents a node in the parse tree
 */
typedef struct node
{

    char *value;          /**< Node textual value         */
    node_type_t type;     /**< Node classification        */
    document_t file;      /**< Source document / location */
    struct node *parent;  /**< Parent node                */
    struct node *child;   /**< First child node           */
    struct node *sibling; /**< Next sibling node          */

} node_t, *ast_t;

/**
 * @brief Check that all statements are properly closed after parsing
 *
 * @return boolean_t Non-zero if all statements are closed, zero otherwise
 */
boolean_t soare_is_all_statement_closed(void);

/**
 * @brief Create a new AST node (branch) with the specified properties
 *
 * @param value node_t textual value (copied or owned according to project rules)
 * @param type node_t type from `node_type_t`
 * @param file document_t context for the node's source location
 * @return node_t* Allocated node, or NULL on allocation failure
 */
node_t *soare_new_node(char *value, node_type_t type, document_t file);

/**
 * @brief Append `element` as a sibling of `source`
 *
 * @param source Existing node to which the element will be juxtaposed
 * @param element node_t or subtree to append as a sibling
 * @return ast_t The resulting AST
 */
ast_t soare_tree_juxtapose(ast_t source, ast_t element);

/**
 * @brief Attach `child` as the child of `parent` and return the modified AST
 *
 * @param parent Parent node to receive the child
 * @param child Child node to attach
 * @return AST The updated AST
 */
ast_t soare_tree_join(ast_t parent, ast_t child);

/**
 * @brief Recursively free an AST and all owned resources
 *
 * @param tree Root of the tree to free
 */
void soare_tree_free(ast_t tree);

/**
 * @brief Parse a token stream into an AST
 *
 * @param tokens Token stream produced by the tokenizer/lexer
 * @return AST Root of the parsed abstract syntax tree, or NULL on parse error
 */
ast_t soare_parser(tokens_t *tokens);

#endif /* __SOARE_PARSER_H__ */