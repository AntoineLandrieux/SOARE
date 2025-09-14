#include <stdio.h>
#include <stdarg.h>
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
 * Antoine LANDRIEUX (MIT License) <Tokenizer.c>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

#include <SOARE/SOARE.h>

/**
 * @brief Check if a character is a number
 *
 * @param character
 *
 * @return unsigned char
 */
static inline unsigned char chrNum(const char character)
{
    return character >= '0' && character <= '9';
}

/**
 * @brief Check if the character is a letter
 *
 * @param character
 * @return unsigned char
 */
static inline unsigned char chrAlpha(const char character)
{
    return (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z');
}

/**
 * @brief Check if the character is a letter or a number
 *
 * @param character
 * @return unsigned char
 */
static inline unsigned char chrAlnum(const char character)
{
    return chrAlpha(character) || chrNum(character);
}

/**
 * @brief Check if the character is a space
 *
 * @param character
 * @return unsigned char
 */
static inline unsigned char chrSpace(const char character)
{
    return character == ' ' || character == '\t' || character == '\r' || character == '\n';
}

/**
 * @brief Check if the character is an operator
 *
 * @param string
 * @return unsigned char
 */
static inline unsigned char chrOperator(const char character)
{
    return strchr("<,+-^*/%>", character) != NULL;
}

/**
 * @brief Check if the string is an operator
 *
 * @param string
 * @return unsigned char
 */
static inline unsigned char strOperator(char *string)
{
    return (
        //
        !strcmp("==", string) ||
        !strcmp("<=", string) ||
        !strcmp(">=", string) ||
        !strcmp("!=", string) ||
        !strcmp("~=", string) || // Same as !=
        !strcmp("&&", string) ||
        !strcmp("||", string)
        //
    );
}

/**
 * @brief Check if the string is a keyword
 *
 * @param string
 * @return unsigned char
 */
static inline unsigned char strKeyword(char *string)
{
    return (
        //
        !strcmp(KEYWORD_DO, string) ||
        !strcmp(KEYWORD_FN, string) ||
        !strcmp(KEYWORD_IF, string) ||
        !strcmp(KEYWORD_OR, string) ||
        !strcmp(KEYWORD_LET, string) ||
        !strcmp(KEYWORD_TRY, string) ||
        !strcmp(KEYWORD_END, string) ||
        !strcmp(KEYWORD_ELSE, string) ||
        !strcmp(KEYWORD_WHILE, string) ||
        !strcmp(KEYWORD_RAISE, string) ||
        !strcmp(KEYWORD_BREAK, string) ||
        !strcmp(KEYWORD_RETURN, string) ||
        !strcmp(KEYWORD_IFERROR, string) ||
        !strcmp(KEYWORD_LOADIMPORT, string) ||
        // Custom keyword
        soare_iskeyword(string)
        //
    );
}

/**
 * @brief Give the type of the string
 *
 * @param string
 * @return token_type
 */
static inline token_type Symbol(char *string)
{
    return strKeyword(string) ? TKN_KEYWORD : TKN_NAME;
}

/**
 * @brief Translate Escape Sequence <https://github.com/AntoineLandrieux/EscapeSequenceC/>
 *
 * @param string
 * @return char*
 */
static char *TranslateEscapeSequence(char *string)
{
    if (!string)
        return NULL;

    char *chr = string;
    char *end = string + strlen(string);

    unsigned int num = 0;
    volatile int len = 1;

    while (NULL != (chr = strchr(chr, '\\')))
    {
        len = 1;

        switch (*(chr + 1))
        {
        case 'e':
            *chr = '\033';
            break;

        case 'n':
            *chr = '\n';
            break;

        case 'f':
            *chr = '\f';
            break;

        case 'r':
            *chr = '\r';
            break;

        case 'a':
            *chr = '\a';
            break;

        case 'v':
            *chr = '\v';
            break;

        case 't':
            *chr = '\t';
            break;

        case 'b':
            *chr = '\b';
            break;

        case 'x':
            if (!sscanf(chr + 2, "%2x", &num))
                return LeaveException(InvalidEscapeSequence, chr, EmptyDocument());
            *chr = (char)num;
            len = 3;
            break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
            if (!sscanf(chr + 1, "%3o", &num))
                return LeaveException(InvalidEscapeSequence, chr, EmptyDocument());
            *chr = (char)num;
            len = 3;
            break;

        case '`':
        case '"':
        case '\'':
        case '\\':
            *chr = *(chr + 1);
            break;

        default:
            return LeaveException(InvalidEscapeSequence, chr, EmptyDocument());
        }

        chr++;
        memmove(chr, chr + len, end - chr + len);
    }

    return string;
}

/**
 * @brief Return an empty document
 *
 * @return Document
 */
Document EmptyDocument(void)
{
    Document document;

    document.file = NULL;
    document.ln = 0;
    document.col = 0;

    return document;
}

/**
 * @brief Create a new token
 *
 * @param filename
 * @param value
 * @param type
 * @return Tokens*
 */
Tokens *Token(char *__restrict__ filename, char *__restrict__ value, token_type type)
{
    Tokens *token = (Tokens *)malloc(sizeof(Tokens));

    if (!token)
        return __SOARE_OUT_OF_MEMORY();

    token->value = !value ? NULL : strdup(value);
    token->type = type;

    token->file.ln = 0;
    token->file.col = 0;
    token->file.file = filename;

    token->next = NULL;

    return token;
}

/**
 * @brief Move on to the next token
 *
 * @param tokens
 * @param step
 */
void TokenNext(Tokens **tokens, unsigned int step)
{
    for (unsigned int _ = 0; _ < step; _++)
        *tokens = (*tokens)->next;
}

/**
 * @brief Check if a sequence of tokens corresponds with a sequence of token types
 *
 * @param tokens
 * @param iteration
 * @param ...
 * @return unsigned char
 */
unsigned char TokensFollowPattern(Tokens *tokens, unsigned int iteration, ...)
{
    va_list args;
    va_start(args, iteration);
    unsigned char result = 1;

    for (unsigned int i = 0; result && i < iteration; i++)
    {
        result = va_arg(args, token_type) == tokens->type;
        tokens = tokens->next;
        result = result && (tokens || i - 1 == iteration);
    }

    va_end(args);
    return result;
}

/**
 * @brief Free the memory allocated by the tokens
 *
 * @param token
 */
void TokensFree(Tokens *token)
{
    if (!token)
        return;

    TokensFree(token->next);
    free(token->value);
    free(token);
}

#ifdef __SOARE_DEBUG

/**
 * @brief Display the tokens
 *
 * @param token
 */
void TokensLog(Tokens *token)
{
    if (!token)
        return;

    /**
     *
     * Example:
     *
     * [TOKENS] [test.soare:00001:00001, 09, "write"]
     * [TOKENS] [test.soare:00001:00007, 03, "Hello"]
     * [TOKENS] [test.soare:00001:00014, 0B, ";"]
     * [TOKENS] [test.soare:00000:00000, 00, "(null)"]
     *
     */

    soare_write(
        __soare_stdout,
        "[TOKENS] [%s:%.5lld:%.5lld, %.2X, \"%s\"]\n",
        token->file.file,
        token->file.ln,
        token->file.col,
        token->type,
        token->value);
    TokensLog(token->next);
}

#endif

/**
 * @brief Cut a string
 *
 * @param string
 * @param size
 * @return char*
 */
static char *strcut(const char *string, size_t size)
{
    if (strlen(string) < size)
        size = strlen(string);

    char *result = (char *)malloc(size + 1);

    if (!result)
        return __SOARE_OUT_OF_MEMORY();

    for (size_t ptr = 0; ptr < size; ptr++)
        result[ptr] = string[ptr];

    result[size] = 0;
    return result;
}

/**
 * @brief Add +1 to ln and set col to 0
 *
 * @param ln
 * @param col
 */
static inline void updateln(unsigned long long *__restrict__ ln, unsigned long long *__restrict__ col)
{
    *ln = (*ln) + 1;
    *col = 1;
}

/**
 * @brief Transform a string into a sequence of tokens
 *
 * @param filename
 * @param text
 * @return Tokens*
 */
Tokens *Tokenizer(char *__restrict__ filename, char *__restrict__ text)
{
    if (!text)
        return NULL;

    Tokens *token = Token(filename, NULL, TKN_EOF);
    Tokens *curr = token;

    // Line/Column
    unsigned long long ln = 0;
    unsigned long long col = 0;
    // Let:
    // ln   = 1
    // col  = 1
    updateln(&ln, &col);

    while (*text)
    {
        // Check for errors
        if (ErrorLevel())
        {
            TokensFree(token);
            return NULL;
        }

        // Ignore space sequence
        if (chrSpace(*text))
        {
            col++;
            // Check if there is a new line
            if (*text == '\n')
                // increment ln
                // col = 1
                updateln(&ln, &col);
            text++;
            continue;
        }

        // Comments
        else if (*text == '?')
        {
            // Comments end with a new line ('\n')
            while (*text != '\n' && *text)
                text++;
            continue;
        }

        token_type type = TKN_EOF;
        unsigned long long offset = 1;

        curr->file.ln = ln;
        curr->file.col = col;

        // Let text = "<="
        char operator[3] = {
            //
            0 [text], // <
            1 [text], // =
            0         // Null character
            //
        };

        // Check if the string (operator) is an operator
        if (strOperator(operator))
        {
            offset += 1;
            type = TKN_OPERATOR;
        }

        // Assign
        else if (*text == '=')
            type = TKN_ASSIGN;

        // Parentheses
        else if (strchr("()", *text))
            type = *text == '(' ? TKN_PARENL : TKN_PARENR;

        // Array
        else if (strchr("[]", *text))
            type = *text == '[' ? TKN_ARRAYL : TKN_ARRAYR;

        // Semicolon or operator
        else if (chrOperator(*text) || *text == ';')
            type = *text == ';' ? TKN_SEMICOLON : TKN_OPERATOR;

        // Name
        else if (chrAlpha(*text) || *text == '_')
            while (chrAlnum((&*text)[offset]) || (&*text)[offset] == '_')
                offset++;

        // Number (int, float)
        else if (chrNum(*text))
        {
            unsigned char point = 0;
            while (chrNum((&*text)[offset]) || ((&*text)[offset] == '.' && !point))
                offset++;
            type = TKN_NUMBER;
        }

        // String `str`|'str'|"str"
        else if (strchr("\"'`", *text) != NULL)
        {
            offset--;
            char ignore = 0;
            char quote = *text;
            text++;

            while ((&*text)[offset] && ((&*text)[offset] != quote || ignore))
            {
                ignore = !ignore && (&*text)[offset] == '\\';
                offset++;
            }

            type = TKN_STRING;
            offset++;
        }

        // Error
        else
        {
            LeaveException(CharacterError, &*text, curr->file);
            continue;
        }

        // Add token
        curr->value = type == TKN_STRING ? TranslateEscapeSequence(strcut(&*text, offset - 1)) : strcut(&*text, offset);
        curr->type = type == TKN_EOF ? Symbol(curr->value) : type;
        curr->next = Token(filename, NULL, TKN_EOF);
        curr = curr->next;

        // Update text pointer
        for (unsigned long long i = 0; i < offset; i++)
        {
            col += 1;
            if (*text == '\n')
                updateln(&ln, &col);
            text++;
        }
        col += type == TKN_STRING;
    }

    // Return token
    return token;
}
