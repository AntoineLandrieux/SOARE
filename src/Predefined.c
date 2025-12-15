#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include <time.h>

#include <SOARE/SOARE.h>

#include "predefined.h"

////////////////////////////////////////////////////////////
static inline char *__byte_to_string(unsigned char byte)
{
    char result[4] = {0, 0, 0, 0};
    snprintf(result, 4, "%d", byte);
    return strdup(result);
}

////////////////////////////////////////////////////////////
char *__soare_eval(soare_arguments_list args)
{
    char *code = soare_getarg(args, 0);

    if (!code)
        return NULL;

    char *value = Execute("<eval>", code);
    free(code);

    return value;
}

////////////////////////////////////////////////////////////
char *__soare_exit(soare_arguments_list args)
{
    char *code = soare_getarg(args, 0);
    int status = !code ? 0 : atoi(code);

    free(code);
    exit(status);

    return NULL;
}

////////////////////////////////////////////////////////////
char *__soare_timestamp(soare_arguments_list args)
{
    (void)args;
    char timestamp[20];
    snprintf(timestamp, sizeof(timestamp), "%lld", (long long int)time(NULL));
    return strdup(timestamp);
}

////////////////////////////////////////////////////////////
char *__soare_system(soare_arguments_list args)
{
    int result = 0;
    char *value = NULL;

    // Loop through all arguments and execute them as system commands
    for (unsigned int i = 0; 1; i++)
    {
        if (!(value = soare_getarg(args, i)))
            break;

        result |= system(value);
        free(value);
    }

    // Allocate string for return value ("0" or "1")
    if ((value = malloc(2)))
    {
        value[0] = !!result + '0';
        value[1] = 0;
    }

    return value;
}

////////////////////////////////////////////////////////////
char *__write(FILE *stream, soare_arguments_list args)
{
    char *value = NULL;

    // Loop through all arguments and print them
    for (unsigned int i = 0; 1; i++)
    {
        if (!(value = soare_getarg(args, i)))
            break;

        fprintf(stream, "%s", value);
        free(value);
    }

    return NULL;
}

////////////////////////////////////////////////////////////
char *__soare_input(soare_arguments_list args)
{
    __write(stdout, args);

    char input[__SOARE_MAX_INPUT__];

    if (!soare_input(input))
        return NULL;

    char *result = strdup(input);

    if (!result)
        return __SOARE_OUT_OF_MEMORY();

    result[strlen(result) - 1] = 0;
    return result;
}

////////////////////////////////////////////////////////////
char *__soare_write(soare_arguments_list args)
{
    return __write(stdout, args);
}

////////////////////////////////////////////////////////////
char *__soare_werr(soare_arguments_list args)
{
    return __write(stderr, args);
}

////////////////////////////////////////////////////////////
char *__soare_random(soare_arguments_list args)
{
    char *seed = soare_getarg(args, 0);

    if (!seed)
        return NULL;

    srand((unsigned int)atoi(seed));
    free(seed);

    return __byte_to_string((unsigned char)rand());
}

////////////////////////////////////////////////////////////
char *__soare_chr(soare_arguments_list args)
{
    char *value = soare_getarg(args, 0);

    if (!value)
        return NULL;

    unsigned char num = (unsigned char)atoi(value);
    free(value);

    char result[2] = {(char)num, 0};

    return strdup(result);
}

////////////////////////////////////////////////////////////
char *__soare_ord(soare_arguments_list args)
{
    char *value = soare_getarg(args, 0);

    if (!value)
        return NULL;

    unsigned char ch = (unsigned char)value[0];
    free(value);

    return __byte_to_string(ch);
}

////////////////////////////////////////////////////////////
void predefined_functions(void)
{
    /* Interpreter */
    assert(soare_addfunction("eval", __soare_eval));
    assert(soare_addfunction("exit", __soare_exit));
    assert(soare_addfunction("system", __soare_system));
    assert(soare_addfunction("time", __soare_timestamp));

    /* Number / String */
    assert(soare_addfunction("random", __soare_random));
    assert(soare_addfunction("chr", __soare_chr));
    assert(soare_addfunction("ord", __soare_ord));

    /* Input / Output */
    assert(soare_addfunction("input", __soare_input));
    assert(soare_addfunction("write", __soare_write));
    assert(soare_addfunction("werr", __soare_werr));
}
