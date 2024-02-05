#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SOARE/SOARE.h>

unsigned char char_operator(const char _Char)
{
    return (
        _Char == '*' ||
        _Char == '^' ||
        _Char == '/' ||
        _Char == '%' ||
        _Char == '+' ||
        _Char == '-' ||
        _Char == '<' ||
        _Char == '>' ||
        _Char == '!' ||
        _Char == '=' ||
        _Char == '|' ||
        _Char == '&');
}

unsigned char char_mathPriority(const char _Op)
{
    if (strchr("/*%%^", _Op) != NULL)
        return 1;
    else if (strchr("+-", _Op) != NULL)
        return 2;
    else if (strchr("<=!>", _Op) != NULL)
        return 3;
    else if (strchr("&|", _Op) != NULL)
        return 4;
    return 0;
}

unsigned char string_isNaN(const char *_String)
{
    if (_String == NULL)
        return 1;
    if (*_String == '-')
        (volatile char *)_String++;
    unsigned char dot = 0;
    for (; *_String; (volatile char *)_String++)
        if (*_String == '.' && !dot)
            dot = 1;
        else if (char_IsNaN(*_String))
            return 1;
    return 0;
}

unsigned char string_keyword(const char *_String)
{
    return (
        !strcasecmp(_String, "LOADIMPORT") ||
        !strcasecmp(_String, "FUNCTION") ||
        !strcasecmp(_String, "RETURN") ||
        !strcasecmp(_String, "GLOBAL") ||
        !strcasecmp(_String, "PROMPT") ||
        !strcasecmp(_String, "WHILE") ||
        !strcasecmp(_String, "PUTS") ||
        !strcasecmp(_String, "ELSE") ||
        !strcasecmp(_String, "IF") ||
        !strcasecmp(_String, "TO"));
}

char *string_slice(const char *_String, const unsigned long long _To)
{
    size_t _Size = MIN(strlen(_String), _To);
    char *_Res = (char *)malloc(_Size + 1);
    if (_Res == NULL)
        return NULL;
    for (size_t p = 0; p < _Size; p++)
        _Res[p] = _String[p];
    _Res[_Size] = ' ';
    _Res[!_Size ? 1 : _Size] = 0;
    return _Res;
}

char *string_eval(char *x, char operator, char * y)
{
    size_t size = sizeof(SOARE_NaN);
    char *res = (char *)malloc(size);

    if (res == NULL || x == NULL || y == NULL)
    {
        free(x);
        free(y);
        free(res);
        return strdup(SOARE_NULL);
    }

    unsigned char str = string_isNaN(x) || string_isNaN(y);
    strcpy(res, SOARE_NaN);
    double calc = 0.f;

    switch (operator)
    {
    case '+':
        if (str)
        {
            res = (char *)malloc(strlen(x) + strlen(y) + 1);
            strcpy(res, x);
            strcat(res, y);
            break;
        }
        calc = atof(x) + atof(y);
        size = int_Size(calc);
        res = (char *)malloc(size);
        snprintf(res, size, "%.*f", float_count(calc), calc);
        break;

    case '-':
        if (str)
            break;
        free(res);
        calc = atof(x) - atof(y);
        size = int_Size(calc);
        res = malloc(size);
        snprintf(res, size, "%.*f", float_count(calc), calc);
        break;

    case '*':
        if (str)
            break;
        free(res);
        calc = atof(x) * atof(y);
        size = int_Size(calc);
        res = malloc(size);
        snprintf(res, size, "%.*f", float_count(calc), calc);
        break;

    case '/':
        if (str)
            break;
        free(res);
        calc = atof(x) / atof(y);
        size = int_Size(calc);
        res = malloc(size);
        snprintf(res, size, "%.*f", float_count(calc), calc);
        break;

    case '%':
        if (str)
            break;
        free(res);
        calc = atoi(x) % atoi(y);
        size = int_Size(calc);
        res = (char *)malloc(size);
        snprintf(res, size, "%d", (int)calc);
        break;

    case '&':
        !str ? snprintf(res, size, "%d", atof(x) && atof(y)) : 0;
        break;

    case '|':
        !str ? snprintf(res, size, "%d", atof(x) || atof(y)) : 0;
        break;

    case '!':
        if (!str)
            snprintf(res, size, "%d", atof(x) != atof(y));
        else
            snprintf(res, size, "%d", strcmp(x, y));
        break;

    case '=':
        if (!str)
            snprintf(res, size, "%d", atof(x) == atof(y));
        else
            snprintf(res, size, "%d", !strcmp(x, y));
        break;

    case '^':
        !str ? snprintf(res, size, "%d", atoi(x) ^ atoi(y)) : 0;
        break;

    case '<':
        if (!str)
            snprintf(res, size, "%d", atof(x) < atof(y));
        else
            snprintf(res, size, "%d", strlen(x) < strlen(y));
        break;

    case '>':
        if (!str)
            snprintf(res, size, "%d", atof(x) > atof(y));
        else
            snprintf(res, size, "%d", strlen(x) > strlen(y));
        break;

    default:
        break;
    };

    free(x);
    free(y);
    return res;
}

int float_count(double _Float)
{
    int count = 0;
    float dp = _Float - (int)_Float;
    for (; dp * 10 != 0.f && count < 0xF; count++)
    {
        dp *= 10;
        dp -= (int)dp;
    }
    return count;
}
