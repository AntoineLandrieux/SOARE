#ifndef __SAE_AST_H__
#define __SAE_AST_H__ 0x1

/**
 * @brief SOARE NodeType
 * @author Antoine LANDRIEUX
 */
typedef enum nodetype
{
    NODE_ROOT,
    NODE_IMPORT,
    NODE_FUNCTION,
    NODE_SECTION,
    NODE_RETURN,
    NODE_PROMPT,
    NODE_CALL,
    NODE_GOTO,
    NODE_BODY,
    NODE_NUMBER,
    NODE_STRING,
    NODE_OPERATOR,
    NODE_GET,
    NODE_SET,
    NODE_OUT,
    NODE_IF,
    NODE_WHILE,
    NODE_GLOBAL,
    NODE_ARRAY
} NodeType;

/**
 * @brief SOARE Ast
 * @author Antoine LANDRIEUX
 */
typedef struct ast
{
    char *_Value;
    NodeType _Type;

    struct ast *_Child;
    struct ast *_Parent;
    struct ast *_SiblingR;
    struct ast *_SiblingL;

    struct ast *_Array;
} Ast, *AST;

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Value
 * @param _Type
 * @return AST
 */
AST branch(char *_Value, NodeType _Type);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Parent
 * @param _Child
 * @return AST
 */
AST branch_push(AST _Parent, AST _Child);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Ast
 */
void branch_free(AST _Ast);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Dest
 * @param _Source
 * @return AST
 */
AST branch_join(AST _Dest, AST _Source);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Ast
 */
void branch_puts(AST _Ast);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Tokens
 * @return AST
 */
AST Parser(TOKENS _Tokens);

#endif /* __SAE_AST_H__ */