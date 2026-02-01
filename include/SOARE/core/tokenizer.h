#ifndef __SOARE_TOKENIZER_H__
#define __SOARE_TOKENIZER_H__ 0x1

/* #pragma once */

/**
 *  _____  _____  ___  ______ _____
 * /  ___||  _  |/ _ \ | ___ \  ___|
 * \ `--. | | | / /_\ \| |_/ / |__
 *  `--. \| | | |  _  ||    /|  __|
 * /\__/ /\ \_/ / | | || |\ \| |___
 * \____/  \___/\_| |_/\_| \_\____/
 *
 * Antoine LANDRIEUX (MIT License) <tokenizer.h>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

/**
 * @enum token_type
 * @brief Kinds of tokens emitted by the tokenizer.
 */
typedef enum token_type
{

    TKN_EOF,      /**< End-of-file marker */
    TKN_NAME,     /**< Identifier / name */
    TKN_NUMBER,   /**< Numeric literal */
    TKN_STRING,   /**< String literal */
    TKN_PARENL,   /**< Left parenthesis '(' */
    TKN_PARENR,   /**< Right parenthesis ')' */
    TKN_ASSIGN,   /**< Assignment operator */
    TKN_KEYWORD,  /**< Language keyword */
    TKN_OPERATOR, /**< Operator symbol (+, -, etc.) */
    TKN_SEMICOLON /**< Statement terminator ';' */

} token_type_t;

/**
 * @struct tokens_t
 * @brief Singly-linked token node used by the parser.
 *
 * The tokenizer produces a linked list of `tokens_t`. Each node contains the
 * token textual `value`, its `type`, the `file` document for diagnostics,
 * and a pointer to the next token.
 */
typedef struct tokens
{

    char *value;         /**< Token text (owned by the token stream) */
    token_type_t type;   /**< Token kind */
    document_t file;     /**< Source location for diagnostics */
    struct tokens *next; /**< Next token in stream */

} tokens_t;

/**
 * @brief Return an empty/default `document_t` value.
 *
 * Useful when a token or node has no meaningful filename/location.
 *
 * @return document_t An empty document descriptor.
 */
document_t soare_empty_document(void);

/**
 * @brief Create a new token node.
 *
 * @param filename Source filename for the token (used in the `document_t`).
 * @param value    Token text (string). Ownership and copying follow project conventions.
 * @param type     Token type from `token_type_t`.
 * @return tokens_t* Newly allocated token node, or NULL on allocation failure.
 */
tokens_t *soare_new_tokens(char *__restrict__ filename, char *__restrict__ value, token_type_t type);

/**
 * @brief Free a token stream and all associated memory.
 *
 * @param token Head of the token list to free.
 */
void soare_tokens_free(tokens_t *tokens);

/**
 * @brief Lexical analysis: transform source text into a token stream.
 *
 * @param filename Logical filename to associate with produced tokens.
 * @param text     Source code buffer to tokenize.
 * @return tokens_t* Head of the produced token list, or NULL on error.
 */
tokens_t *soare_tokenizer(char *__restrict__ filename, char *__restrict__ text);

#endif /* __SOARE_TOKENIZER_H__ */