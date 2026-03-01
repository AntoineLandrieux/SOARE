#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SOARE/SOARE.h>

#include "module.h"

/**
 *  _____  _____  ___  ______ _____
 * /  ___||  _  |/ _ \ | ___ \  ___|
 * \ `--. | | | / /_\ \| |_/ / |__
 *  `--. \| | | |  _  ||    /|  __|
 * /\__/ /\ \_/ / | | || |\ \| |___
 * \____/  \___/\_| |_/\_| \_\____/
 *
 * Antoine LANDRIEUX (MIT License) <Module.c>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

static char *__int_to_string(int num)
{
#define MAX_INT_STRING_SIZE 16

    char res[MAX_INT_STRING_SIZE] = {0};
    snprintf(res, MAX_INT_STRING_SIZE, "%d", num);

    char *returns = strdup(res);

    if (!returns)
    {
        SOARE_OUT_OF_MEMORY();
        return NULL;
    }

    return returns;
}

////////////////////////////////////////////////////////////
char *__soare_eval(soare_arguments_list_t args)
{
    char *code = soare_get_argument(args, 0);

    if (!code)
    {
        return NULL;
    }

    char *value = soare_execute("<eval>", code);
    free(code);

    return value;
}

////////////////////////////////////////////////////////////
char *__soare_exit(soare_arguments_list_t args)
{
    char *code = soare_get_argument(args, 0);
    int status = !code ? 0 : atoi(code);

    free(code);
    exit(status);

    return NULL;
}

////////////////////////////////////////////////////////////
char *__soare_timestamp(soare_arguments_list_t args)
{
    (void)args;
    char timestamp[20];
    snprintf(timestamp, sizeof(timestamp), "%lld", (long long int)time(NULL));
    return strdup(timestamp);
}

////////////////////////////////////////////////////////////
char *__soare_system(soare_arguments_list_t args)
{
    int errorlevel = 0;
    char *commands = soare_get_argument(args, 0);

    if (!commands)
    {
        return NULL;
    }

    errorlevel = system(commands);
    free(commands);

    return __int_to_string(errorlevel);
}

////////////////////////////////////////////////////////////
char *__write(FILE *stream, soare_arguments_list_t args)
{
    char *value = NULL;

    // Loop through all arguments and print them
    for (unsigned int i = 0; 1; i++)
    {
        if (!(value = soare_get_argument(args, i)))
        {
            break;
        }

        soare_write(stream, "%s", value);
        free(value);
    }

    return NULL;
}

////////////////////////////////////////////////////////////
char *__soare_input(soare_arguments_list_t args)
{
#define MAX_INPUT 255
    char input[MAX_INPUT];

    __write(__soare_stdout, args);

    if (!soare_input(input, MAX_INPUT))
    {
        return NULL;
    }

    char *result = strdup(input);

    if (!result)
    {
        SOARE_OUT_OF_MEMORY();
        return NULL;
    }

    result[strlen(result) - 1] = 0;
    return result;
}

////////////////////////////////////////////////////////////
char *__soare_write(soare_arguments_list_t args)
{
    return __write(__soare_stdout, args);
}

////////////////////////////////////////////////////////////
char *__soare_werr(soare_arguments_list_t args)
{
    return __write(__soare_stderr, args);
}

////////////////////////////////////////////////////////////
char *__soare_random(soare_arguments_list_t args)
{
    char *seed = soare_get_argument(args, 0);

    if (seed)
    {
        srand((unsigned int)atoi(seed));
        free(seed);
    }
    else
    {
        srand((unsigned int)time(NULL));
    }

    return __int_to_string(rand());
}

////////////////////////////////////////////////////////////
char *__soare_define(soare_arguments_list_t args)
{
    char *varname = soare_get_argument(args, 0);
    char *content = soare_get_argument(args, 1);
    char *mutable = soare_get_argument(args, 2);
    boolean_t mut = bFalse;

    if (!varname || soare_errorlevel())
    {
        free(varname);
        free(content);
        free(mutable);
        return NULL;
    }

    if (mutable)
    {
        mut = (boolean_t)atoi(mutable);
    }

    soare_add_variable(varname, content, mut);

    free(varname);
    free(content);
    free(mutable);
    return NULL;
}

////////////////////////////////////////////////////////////
char *__soare_chr(soare_arguments_list_t args)
{
    char *value = soare_get_argument(args, 0);

    if (!value)
    {
        return NULL;
    }

    char num = (char)atoi(value);
    free(value);

    char result[2] = {num, 0};

    return strdup(result);
}

////////////////////////////////////////////////////////////
char *__soare_ord(soare_arguments_list_t args)
{
    char *value = soare_get_argument(args, 0);

    if (!value)
    {
        return NULL;
    }

    int ch = (int)value[0];
    free(value);

    return __int_to_string(ch);
}

////////////////////////////////////////////////////////////
void load_module(void)
{
    soare_add_function("eval" /*    */, __soare_eval);
    soare_add_function("exit" /*    */, __soare_exit);
    soare_add_function("system" /*  */, __soare_system);
    soare_add_function("time" /*    */, __soare_timestamp);
    soare_add_function("random" /*  */, __soare_random);
    soare_add_function("def" /*     */, __soare_define);
    soare_add_function("chr" /*     */, __soare_chr);
    soare_add_function("ord" /*     */, __soare_ord);
    soare_add_function("input" /*   */, __soare_input);
    soare_add_function("write" /*   */, __soare_write);
    soare_add_function("werr" /*    */, __soare_werr);
    soare_add_variable("OS" /*      */, __PLATFORM__ /*   */, bFalse);
    soare_add_variable("false" /*   */, "0" /*            */, bFalse);
    soare_add_variable("true" /*    */, "1" /*            */, bFalse);
    soare_add_variable("null" /*    */, "" /*             */, bFalse);
    soare_add_variable("void" /*    */, NULL /*           */, bFalse);
    soare_add_variable("version" /* */, SOARE_VERSION /*  */, bFalse);
}
