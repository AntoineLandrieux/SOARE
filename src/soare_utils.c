#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SOARE/SOARE.h>

/*
 _____  _____  ___  ______ _____
/  ___||  _  |/ _ \ | ___ \  ___|
\ `--. | | | / /_\ \| |_/ / |__
 `--. \| | | |  _  ||    /|  __|
/\__/ /\ \_/ / | | || |\ \| |___
\____/  \___/\_| |_/\_| \_\____/

 * Antoine LANDRIEUX
 * GNU General Public License v3.0
 * https://www.gnu.org/licenses/
 * 
 * SOARE
 * soare_utils.c
*/

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
        return 0;
    else if (strchr("+-", _Op) != NULL)
        return 1;
    else if (strchr("<=!>", _Op) != NULL)
        return 2;
    else if (strchr("&|", _Op) != NULL)
        return 3;
    return 4;
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
    char *_Result = (char *)malloc(_Size + 1);
    if (_Result == NULL)
        return NULL;
    for (size_t p = 0; p < _Size; p++)
        _Result[p] = _String[p];
    _Result[_Size] = ' ';
    _Result[!_Size ? 1 : _Size] = 0;
    return _Result;
}

static char *string_prepare(char *_String, double _Calc)
{
    free(_String);
    _String = NULL;
    size_t _Size = int_Size(_Calc);
    _String = (char *)malloc(_Size);
    if (_String != NULL)
        snprintf(_String, _Size, "%.*f", float_count(_Calc), _Calc);
    return _String;
}

char *string_eval(char *x, char _Operator, char * y)
{
    size_t _Size = sizeof(SOARE_NaN);
    char *_Result = malloc(_Size);

    if (_Result == NULL || x == NULL || y == NULL)
    {
        free(x);
        free(y);
        free(_Result);
        return strdup(SOARE_NULL);
    }

    strcpy(_Result, SOARE_NaN);
    unsigned char str = string_isNaN(x) || string_isNaN(y);

    switch (_Operator)
    {
    case '+':
        if (!str)
        {
            _Result = string_prepare(_Result, atof(x) + atof(y));
            break;
        }
        _Result = (char *)malloc(strlen(x) + strlen(y) + 1);
        strcpy(_Result, x);
        strcat(_Result, y);
        break;

    case '-':
        if (str)
            break;
        _Result = string_prepare(_Result, atof(x) - atof(y));
        break;

    case '*':
        if (str)
            break;
        _Result = string_prepare(_Result, atof(x) * atof(y));
        break;

    case '/':
        if (str)
            break;
        _Result = string_prepare(_Result, atof(x) / atof(y));
        break;

    case '%':
        if (str)
            break;
        _Result = string_prepare(_Result, atoi(x) % atoi(y));
        break;
    
    case '^':
        if (str)
            break;
        _Result = string_prepare(_Result, atoi(x) ^ atoi(y));
        break;

    case '&':
        !str ? snprintf(_Result, _Size, "%d", atof(x) && atof(y)) : 0;
        break;

    case '|':
        !str ? snprintf(_Result, _Size, "%d", atof(x) || atof(y)) : 0;
        break;

    case '!':
        if (!str)
            snprintf(_Result, _Size, "%d", atof(x) != atof(y));
        else
            snprintf(_Result, _Size, "%d", strcmp(x, y));
        break;

    case '=':
        if (!str)
            snprintf(_Result, _Size, "%d", atof(x) == atof(y));
        else
            snprintf(_Result, _Size, "%d", !strcmp(x, y));
        break;

    case '<':
        if (!str)
            snprintf(_Result, _Size, "%d", atof(x) < atof(y));
        else
            snprintf(_Result, _Size, "%d", strlen(x) < strlen(y));
        break;

    case '>':
        if (!str)
            snprintf(_Result, _Size, "%d", atof(x) > atof(y));
        else
            snprintf(_Result, _Size, "%d", strlen(x) > strlen(y));
        break;

    default:
        break;
    };

    free(x);
    free(y);
    return EQU(NULL, _Result, strdup(SOARE_NULL));
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
