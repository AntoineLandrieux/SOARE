#include <stdio.h>
#include <stdlib.h>

#include <SOARE/SOARE.h>

TOKENS token(char *_Value, TokenType _Type, unsigned long long _Ln, unsigned long long _Col)
{
    TOKENS token = (TOKENS)malloc(sizeof(struct tokens));
    if (token == NULL)
        return NULL;
    token->_Value = _Value;
    token->_Type = _Type;
    token->_Ln = _Ln;
    token->_Col = _Col;
    token->_Next = NULL;
    return token;
}

void token_free(TOKENS _Token)
{
    if (_Token == NULL)
        return;
    token_free(_Token->_Next);
    if (_Token->_Value != NULL)
        free(_Token->_Value);
    free(_Token);
}

TOKENS token_join(TOKENS _Dest, TOKENS _Source)
{
    if (_Source == NULL || _Dest == NULL)
        return EQU(NULL, _Source, _Dest);
    TOKENS tmp = _Dest;
    if (tmp->_Next == NULL)
        return _Source;
    while (tmp->_Next->_Next != NULL)
        tmp = tmp->_Next;
    token_free(tmp->_Next);
    tmp->_Next = _Source;
    return _Dest;
}

void token_puts(TOKENS _Token)
{
    if (_Token == NULL)
        return;
    fprintf(stdout,
            "[TOKEN] 0x%x, file:%lld:%lld,\t\"%s\"\n",
            _Token->_Type,
            _Token->_Ln,
            _Token->_Col,
            _Token->_Value);
    token_puts(_Token->_Next);
}

static void update_line(size_t *ln, size_t *col)
{
    *ln += 1;
    *col = 1;
}

TOKENS Tokenizer(const char *_RawCode)
{
    if (_RawCode == NULL)
        return NULL;

    static size_t col = 0;
    static size_t ln = 1;
    update_line(&ln, &col);
    ln -= 1;

    TOKENS tokens = token(NULL, EOF, ln, col);
    TOKENS tok = tokens;

    while (*_RawCode)
    {
        if (*_RawCode == '~')
        {
            for ((volatile char *)_RawCode++; !char_Bckln(*_RawCode); (volatile char *)_RawCode++)
                if (!*_RawCode)
                    break;
            continue;
        }

        if (char_Space(*_RawCode))
        {
            char_Bckln(*_RawCode) ? update_line(&ln, &col) : col++;
            (volatile char *)_RawCode++;
            continue;
        }

        size_t _Adder = 1;
        TokenType _Type = TOKEN_EOF;

        if (*_RawCode == ';')
            _Type = TOKEN_DELIMITER;

        else if (*_RawCode == '@' || *_RawCode == '$')
            _Type = *_RawCode == '@' ? TOKEN_SECTION : TOKEN_VARIABLE;

        else if (*_RawCode == '[' || *_RawCode == ']')
            _Type = *_RawCode == '[' ? TOKEN_LARRAY : TOKEN_RARRAY;

        else if (*_RawCode == '(' || *_RawCode == ')')
            _Type = *_RawCode == '(' ? TOKEN_LPAREN : TOKEN_RPAREN;

        else if (*_RawCode == '{' || *_RawCode == '}')
            _Type = *_RawCode == '{' ? TOKEN_OPEN : TOKEN_CLOSE;

        else if (char_operator(*_RawCode))
            _Type = TOKEN_OPERATOR;

        else if (!char_IsNaN(*_RawCode))
        {
            char *_Slice = string_slice(&*_RawCode, _Adder);
            while (!string_isNaN(_Slice))
            {
                _Adder++;
                free(_Slice);
                if (!*&_RawCode[_Adder])
                    break;
                _Slice = string_slice(&*_RawCode, _Adder);
            }
            _Adder--;
            _Type = TOKEN_NUMBER;
        }

        else if (char_Alpha(*_RawCode) || *_RawCode == '_')
        {
            while (char_AlNum(*&_RawCode[_Adder]) || *&_RawCode[_Adder] == '_')
                _Adder++;
            char *_Slice = string_slice(&*_RawCode, _Adder);
            _Type = string_keyword(_Slice) ? TOKEN_KEYWORD : TOKEN_NAME;
            free(_Slice);
        }

        else if (char_Quote(*_RawCode))
        {
            char quote = *_RawCode;
            (volatile char *)_RawCode++;
            _Adder--;
            while (*&_RawCode[_Adder] && *&_RawCode[_Adder] != quote)
                _Adder++;
            _Type = TOKEN_STRING;
        }

        else
        {
            char *At = string_slice(&*_RawCode, 1);
            ThrowsLang(ERROR_CHARACTER, At, ln, col);
            free(At);
            token_free(tokens);
            return NULL;
        }

        tok->_Value = string_slice(&*_RawCode, _Adder);
        tok->_Type = _Type;
        tok->_Ln = ln;
        tok->_Col = col;

        tok->_Next = token(NULL, TOKEN_EOF, 0, 0);
        tok = tok->_Next;

        for (size_t i = 0; i != (_Adder + (_Type == TOKEN_STRING)) && *_RawCode; i++)
        {
            char_Bckln(*_RawCode) ? update_line(&ln, &col) : col++;
            (volatile char *)_RawCode++;
        }
    }

    tok->_Value = NULL;
    tok->_Type = TOKEN_EOF;

    return tokens;
}

TOKENS file_read(const char *_File)
{
    TOKENS tokens = NULL;
    FILE *file = NULL;

    file = fopen(_File, "r+");

    if (file == NULL)
    {
        ThrowsLang(ERROR_FILE, _File, 0x0, 0x0);
        return NULL;
    }

    char user[SOARE_MAX_INPUT];

    while (fgets(user, SOARE_MAX_INPUT, file))
        tokens = token_join(tokens, Tokenizer(user));
    
    fclose(file);
    return tokens;
}
