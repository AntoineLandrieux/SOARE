#ifndef __SAE_TOKEN_H__
#define __SAE_TOKEN_H__ 0x1

/**
 * @brief SOARE TokenType
 * @author Antoine LANDRIEUX
 */
typedef enum tokentype
{
    TOKEN_EOF,
    TOKEN_KEYWORD,
    TOKEN_NAME,
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_DELIMITER,
    TOKEN_VARIABLE,
    TOKEN_SECTION,
    TOKEN_LARRAY,
    TOKEN_RARRAY,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_OPEN,
    TOKEN_CLOSE
} TokenType;

/**
 * @brief SOARE Tokens
 * @author Antoine LANDRIEUX
 */
typedef struct tokens
{
    char *_Value;
    TokenType _Type;

    unsigned long long _Ln;
    unsigned long long _Col;

    struct tokens *_Next;
} Tokens, *TOKENS;

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Value
 * @param _Type
 * @return TOKENS
 */
TOKENS token(char *_Value, TokenType _Type, unsigned long long _Ln, unsigned long long _Col);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Token
 */
void token_free(TOKENS _Token);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Dest
 * @param _Source
 * @return TOKENS
 */
TOKENS token_join(TOKENS _Dest, TOKENS _Source);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Token
 */
void token_puts(TOKENS _Token);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _RawCode
 * @return TOKENS
 */
TOKENS Tokenizer(const char *_RawCode);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _File
 * @return TOKENS
 */
TOKENS file_read(const char *_File);

#endif /* __SAE_TOKEN_H__ */