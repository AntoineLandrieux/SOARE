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

////////////////////////////////////////////////////////////
static inline unsigned char chr_num(const char character)
{
    return (character >= '0' && character <= '9') || character == '.';
}

////////////////////////////////////////////////////////////
static inline unsigned char chr_alpha_(const char character)
{
    return ((character >= 'a' && character <= 'z') ||
            (character >= 'A' && character <= 'Z') ||
            character == '_');
}

////////////////////////////////////////////////////////////
static inline unsigned char chr_alnum_(const char character)
{
    return chr_alpha_(character) || chr_num(character);
}

////////////////////////////////////////////////////////////
static inline unsigned char chr_space(const char character)
{
    return (character == ' ' || character == '\t' ||
            character == '\r' || character == '\n');
}

////////////////////////////////////////////////////////////
static inline unsigned char chr_operator(const char character)
{
    return strchr("<,:+-^*/%>", character) != NULL;
}

////////////////////////////////////////////////////////////
static inline unsigned char str_operator(char *string)
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

////////////////////////////////////////////////////////////
static inline unsigned char str_keyword(char *string)
{
    return (
        //
        !strcmp(KEYWORD_AS, string) ||
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
        soare_get_keyword(string)
        //
    );
}

////////////////////////////////////////////////////////////
static inline token_type_t symbol(char *string)
{
    return str_keyword(string) ? TKN_KEYWORD : TKN_NAME;
}

////////////////////////////////////////////////////////////
static void translate_escape_sequence(tokens_t *token)
{
    // Translate Escape Sequence
    // <https://github.com/AntoineLandrieux/EscapeSequenceC/>

    if (!token)
        return;

    char *string = token->value;

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
            {
                soare_leave_exception(InvalidEscapeSequence, chr, token->file);
                return;
            }
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
            {
                soare_leave_exception(InvalidEscapeSequence, chr, token->file);
                return;
            }
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
            soare_leave_exception(InvalidEscapeSequence, chr, token->file);
            return;
        }

        chr++;
        memmove(chr, chr + len, end - chr + len);
    }
}

////////////////////////////////////////////////////////////
document_t soare_empty_document(void)
{
    document_t document;

    document.filename = NULL;
    document.ln = 0;
    document.col = 0;

    return document;
}

////////////////////////////////////////////////////////////
tokens_t *soare_new_tokens(char *__restrict__ filename, char *__restrict__ value, token_type_t type)
{
    tokens_t *token = (tokens_t *)malloc(sizeof(tokens_t));

    if (!token)
        return __SOARE_OUT_OF_MEMORY();

    token->value = !value ? NULL : strdup(value);
    token->type = type;

    token->file.ln = 0;
    token->file.col = 0;
    token->file.filename = filename;

    token->next = NULL;

    return token;
}

////////////////////////////////////////////////////////////
void soare_tokens_free(tokens_t *token)
{
    if (!token)
        return;

    soare_tokens_free(token->next);
    free(token->value);
    free(token);
}

////////////////////////////////////////////////////////////
static char *strcut(const char *string, size_t size)
{
    // Cut a string
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

////////////////////////////////////////////////////////////
static inline void updateln(unsigned long long *__restrict__ ln, unsigned long long *__restrict__ col)
{
    // Add +1 to ln and set col to 0
    *ln = (*ln) + 1;
    *col = 1;
}

////////////////////////////////////////////////////////////
tokens_t *soare_tokenizer(char *__restrict__ filename, char *__restrict__ text)
{
    if (!text)
        return NULL;

    tokens_t *token = soare_new_tokens(filename, NULL, TKN_EOF);
    tokens_t *curr = token;

    // Line/Column
    unsigned long long ln = 1;
    unsigned long long col = 1;

    while (*text)
    {
        // Check for errors
        if (soare_errorlevel())
        {
            soare_tokens_free(token);
            return NULL;
        }

        // Ignore space sequence
        if (chr_space(*text))
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

        token_type_t type = TKN_EOF;
        unsigned long long offset = 1;

        curr->file.ln = ln;
        curr->file.col = col;

        // Let text = "<="
        char operator[3] = {
            //
            text[0], // <
            text[1], // =
            0        // Null character
            //
        };

        // Check if the string (operator) is an operator
        if (str_operator(operator))
        {
            offset += 1;
            type = TKN_OPERATOR;
        }

        // Assign
        else if (*text == '=')
            type = TKN_ASSIGN;

        // Parenthesis
        else if (*text == '(')
            type = TKN_PARENL;
        else if (*text == ')')
            type = TKN_PARENR;

        // Semicolon
        else if (*text == ';')
            type = TKN_SEMICOLON;

        // Operator
        else if (chr_operator(*text))
            type = TKN_OPERATOR;

        // Name
        else if (chr_alpha_(*text))
        {
            for (; chr_alnum_(text[offset]); offset++)
                /* pass */;
        }

        // Number (int, float)
        else if (chr_num(*text))
        {
            for (type = TKN_NUMBER; chr_num(text[offset]); offset++)
                /* pass */;
        }

        // String `str`|'str'|"str"
        else if (strchr("\"'`", *text) != NULL)
        {
            char ignore = 0;
            char quote = *text;

            offset--;
            text++;

            while (text[offset] && (text[offset] != quote || ignore))
            {
                ignore = !ignore && text[offset] == '\\';
                offset++;
            }

            type = TKN_STRING;
        }

        // Error: `CharacterError`
        else
        {
            soare_leave_exception(CharacterError, text, curr->file);
            continue;
        }

        // Add token
        curr->value = strcut(text, offset);
        if (type == TKN_STRING)
            translate_escape_sequence(curr);

        curr->type = !type ? symbol(curr->value) : type;
        curr->next = soare_new_tokens(filename, NULL, TKN_EOF);

        curr = curr->next;

        offset += type == TKN_STRING;

        // Update text pointer
        for (size_t i = 0; i < offset; i++)
        {
            col++;
            if (*text == '\n')
                updateln(&ln, &col);
            text++;
        }

        col += type == TKN_STRING;
    }

    // Return token
    return token;
}
