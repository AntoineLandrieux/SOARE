#ifndef __SOARE_TOKENIZER_H__
#define __SOARE_TOKENIZER_H__

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
 * @brief Kinds of tokens used by the interpretor
 */
typedef enum token_type
{

    TKN_EOF,      /**< End-Of-File marker           */
    TKN_NAME,     /**< Identifier / name            */
    TKN_NUMBER,   /**< Numeric literal              */
    TKN_STRING,   /**< String literal               */
    TKN_PARENL,   /**< Left parenthesis '('         */
    TKN_PARENR,   /**< Right parenthesis ')'        */
    TKN_ASSIGN,   /**< Assignment operator          */
    TKN_KEYWORD,  /**< Language keyword             */
    TKN_OPERATOR, /**< Operator symbol (+, -, etc.) */
    TKN_SEMICOLON /**< Statement terminator ';'     */

} token_type_t;

/**
 * @struct tokens_t
 * @brief Token node used by the parser
 */
typedef struct tokens
{

    char *value;         /**< Token text                      */
    token_type_t type;   /**< Token value                     */
    document_t file;     /**< Source location for diagnostics */
    struct tokens *next; /**< Next token                      */

} tokens_t;

/**
 * @brief Return an empty/default `document_t` value
 *
 * @return document_t An empty document descriptor
 */
document_t soare_empty_document(void);

/**
 * @brief Create a new token node
 *
 * @param filename Source filename for the token
 * @param value Token text
 * @param type Token type
 * @return tokens_t* Allocated token node, or NULL on allocation failure
 */
tokens_t *soare_new_tokens(char *__restrict__ filename, char *__restrict__ value, token_type_t type);

/**
 * @brief Advance to the next token
 *
 * @param tokens Pointer to the current token pointer
 */
void soare_tokens_next(tokens_t **tokens);

/**
 * @brief Free a token stream and all associated memory
 *
 * @param token Head of the token list to free
 */
void soare_tokens_free(tokens_t *tokens);

/**
 * @brief Lexical analysis: transform source text into a token stream
 *
 * @param filename Logical filename to associate with produced tokens
 * @param text Source code buffer to tokenize
 * @return tokens_t* Head of the produced token list
 */
tokens_t *soare_tokenizer(char *__restrict__ filename, char *__restrict__ text);

#endif /* __SOARE_TOKENIZER_H__ */