#ifndef __SAE_RUN_H__
#define __SAE_RUN_H__ 0x1

/**
 * @brief SOARE memory
 * @author Antoine LANDRIEUX
 */
typedef struct memory
{
    char *_Name;
    char *_Value;
    AST _Access;
    struct memory *_Next;
} Memory, *MEMORY;

/**
 * @brief SOARE resultlang
 * @author Antoine LANDRIEUX
 */
typedef struct resultlang
{
    AST _Ast;
    MEMORY _Memory;
} ResultLang, *RESULTLANG;

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Name
 * @param _Value
 * @param _Access
 * @return MEMORY
 */
MEMORY memory(char *_Name, char *_Value, AST _Access);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Memory
 * @param _Name
 * @param _Access
 * @return MEMORY
 */
MEMORY memory_get(MEMORY _Memory, char *_Name, AST _Access);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Dest
 * @param _Memory
 * @return MEMORY
 */
MEMORY memory_push(MEMORY _Dest, MEMORY _Memory);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Memory
 * @param _Name
 * @param _Value
 * @param _Access
 * @return MEMORY
 */
MEMORY memory_set(MEMORY _Memory, char *_Name, char *_Value, AST _Access);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Memory
 */
void memory_free(MEMORY _Memory);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Memory
 */
void memory_puts(MEMORY _Memory);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Ast
 * @param _Memory
 * @return RESULTLANG
 */
RESULTLANG resultlang(AST _Ast, MEMORY _Memory);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Resultlang
 */
void resultlang_free(RESULTLANG _Resultlang);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Resultlang
 * @return RESULTLANG
 */
RESULTLANG resultlang_puts(RESULTLANG _Resultlang);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Ast
 * @return char*
 */
char *EvalAst(AST _Root, AST _Ast);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Tokens
 * @return RESULTLANG
 */
RESULTLANG ExecuteTokens(TOKENS _Tokens);

#define ExecuteRawCode(_RawCode) (ExecuteTokens(Tokenizer(_RawCode)))

#endif /* __SAE_RUN_H__ */