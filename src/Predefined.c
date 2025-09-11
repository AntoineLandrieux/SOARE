#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#include <SOARE/SOARE.h>

#include "predefined.h"

/**
 * @brief Get current timestamp
 *
 * @param args List of string arguments (commands to execute)
 * @return char* Current timestamp as string
 */
char *__soare_timestamp(soare_arguments_list args)
{
    char timestamp[20];
    snprintf(timestamp, sizeof(timestamp), "%lld", time(NULL));
    return strdup(timestamp);
}

/**
 * @brief Executes each argument as a system command.
 *
 * Iterates through the argument list, calling system() for each argument.
 * Returns "1" if any command fails, "0" otherwise.
 *
 * @param args List of string arguments (commands to execute)
 * @return char* "0" or "1" as a string (allocated, caller must free)
 */
char *__soare_system(soare_arguments_list args)
{
    int result = 0;
    char *value = NULL;

    // Loop through all arguments and execute them as system commands
    for (int i = 0; 1; i++)
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

/**
 * @brief Writes all arguments to the specified stream.
 *
 * Iterates through the argument list and prints each to the given FILE stream.
 *
 * @param stream Output stream (stdout or stderr)
 * @param args List of string arguments to write
 * @return char* Always returns NULL
 */
char *__write(FILE *stream, soare_arguments_list args)
{
    char *value = NULL;

    // Loop through all arguments and print them
    for (int i = 0; 1; i++)
    {
        if (!(value = soare_getarg(args, i)))
            break;

        fprintf(stream, "%s", value);
        free(value);
    }

    return NULL;
}

/**
 * @brief Writes arguments to stdout and reads a line from stdin.
 *
 * Prints all arguments, then reads user input from stdin.
 *
 * @param args List of string arguments to prompt
 * @return char* User input string (allocated, caller must free)
 */
char *__soare_input(soare_arguments_list args)
{
    __write(stdout, args);

    char input[__SOARE_MAX_INPUT__];
    soare_input(input);

    char *result = strdup(input);

    if (!result)
        return __SOARE_OUT_OF_MEMORY();

    result[strlen(result) - 1] = 0;
    return result;
}

/**
 * @brief Writes arguments to stdout.
 *
 * @param args List of string arguments to write
 * @return char* Always returns NULL
 */
char *__soare_write(soare_arguments_list args)
{
    return __write(stdout, args);
}

/**
 * @brief Writes arguments to stderr.
 *
 * @param args List of string arguments to write
 * @return char* Always returns NULL
 */
char *__soare_werr(soare_arguments_list args)
{
    return __write(stderr, args);
}

char *__soare_soareinfo(soare_arguments_list args)
{
    printf(
        //
        "\nSOARE version %s :\n\n"
        "  -> Author: Antoine LANDRIEUX\n"
        "  -> Repository: <https://github.com/AntoineLandrieux/SOARE>\n"
        "  -> Major: %d, Minor: %d, Patch: %d\n"
        "  -> Platform: %s\n"
        "  -> Date: %s\n"
        "  -> Time: %s\n"
        "  -> Timestamp: %s\n"
        "  -> Error level: %d\n"
        "  -> License:\n\n"
        "MIT License\n\n"
        "Copyright (c) 2025 AntoineLandrieux\n\n"
        "Permission is hereby granted, free of charge, to any person obtaining a copy\n"
        "of this software and associated documentation files (the \"Software\"), to deal\n"
        "in the Software without restriction, including without limitation the rights\n"
        "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
        "copies of the Software, and to permit persons to whom the Software is\n"
        "furnished to do so, subject to the following conditions:\n"
        "The above copyright notice and this permission notice shall be included in all\n"
        "copies or substantial portions of the Software.\n"
        "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
        "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
        "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
        "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
        "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
        "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
        "SOFTWARE.\n",
        //
        SOARE_VERSION,
        SOARE_MAJOR, SOARE_MINOR, SOARE_PATCH,
        __PLATFORM__,
        __DATE__,
        __TIME__,
        __TIMESTAMP__,
        (int)ErrorLevel()
        //
    );
    return NULL;
}

/**
 * @brief Random number.
 *
 * @param args List of string arguments to write
 * @return char* Random number as string
 */
char *__soare_random(soare_arguments_list args)
{
    srand((unsigned int)time(NULL));

    int random = rand() % 100;
    char result[4] = {0};

    snprintf(result, 4, "%d", random);

    return strdup(result);
}

/**
 * @brief Character from ASCII code.
 *
 * @param args List of string arguments to write
 * @return char* Character as string
 */
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

/**
 * @brief ASCII code from character.
 *
 * @param args List of string arguments to write
 * @return char* ASCII code as string
 */
char *__soare_ord(soare_arguments_list args)
{
    char *value = soare_getarg(args, 0);

    if (!value)
        return NULL;

    char result[4] = {0, 0, 0, 0};

    snprintf(result, 4, "%d", (unsigned char)value[0]);
    free(value);

    return strdup(result);
}

/**
 * @brief Registers predefined functions with the SOARE runtime.
 *
 * Adds system, input, write, ... functions.
 */
void predefined_functions(void)
{
    /* Interpreter */

    soare_addfunction("soareinfo", __soare_soareinfo);
    soare_addfunction("system", __soare_system);
    soare_addfunction("time", __soare_timestamp);

    /* Number / String */

    soare_addfunction("random", __soare_random);

    soare_addfunction("chr", __soare_chr);
    soare_addfunction("ord", __soare_ord);

    /* Input / Output */

    soare_addfunction("input", __soare_input);
    soare_addfunction("write", __soare_write);
    soare_addfunction("werr", __soare_werr);
}
