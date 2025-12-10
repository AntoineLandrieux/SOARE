#include <stdio.h>
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
 * Antoine LANDRIEUX (MIT License) <Custom.c>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

#include <SOARE/SOARE.h>

/*

==============================================================
Example: Custom Function - Add Numbers with Variable Arguments
==============================================================

This example demonstrates how to implement a custom function
that adds together an unknown number of integer arguments.

----------------------------------------------------------
Function Name:
    char *int_add(soare_arguments_list args)

Arguments:
    - args: A linked list of arguments passed to the function.
        Use soare_getarg(args, i) to retrieve the i-th argument
        as a string.

Return Value:
    - Returns NULL (no value returned to SOARE).
    - If you want to return a value, allocate memory for the
        result string before returning.

----------------------------------------------------------
Implementation Steps:
    1. Initialize result accumulator.
    2. Loop through arguments using soare_getarg.
    3. Convert each argument from string to integer.
    4. Add to result.
    5. Print the result.
    6. Return NULL (or a string if needed).

----------------------------------------------------------
Memory Management:
    - If returning a string, always allocate memory for it.
    - Example:
        char *ret = malloc(6 * sizeof(char));
        if (!ret)
            return NULL;
        strcpy(ret, "value");
        return ret;

----------------------------------------------------------
Code:
----------------------------------------------------------

char *int_add(soare_arguments_list args)
{
    // Accumulator for the sum
    int result = 0;
    // Pointer to current argument string
    char *x = NULL;

    // Loop through all arguments
    for (int i = 0; 1; i++)
    {
        // Retrieve the i-th argument
        x = soare_getarg(args, i);
        if (!x)
            // Exit loop if no more arguments
            break;

        // Convert argument to integer and add to result
        result += atoi(x);
    }

    // Output the result to the console
    printf("%d", result);

    // Return NULL (no value returned to SOARE)
    // If you need to return a value, allocate memory as shown above
    return NULL;
}

Implement this function: soare_addfunction(<function name>, <function>)

soare_addfunction("int_add", int_add);

----------------------------------------------------------

*/

/* Functions */
static struct soare_functions functions_list[100];
/* Count */
static unsigned long functions_count = 0;

////////////////////////////////////////////////////////////
unsigned long soare_addfunction(char *name, char *(*function)(soare_arguments_list))
{
    if (functions_count >= 99 || !name || !function)
        return 0;

    struct soare_functions fn = {name, function};

    functions_list[functions_count] = fn;
    functions_list[functions_count + 1].name = NULL;

    functions_count = functions_count + 1;
    return functions_count;
}

////////////////////////////////////////////////////////////
soare_function soare_getfunction(char *name)
{
    static soare_function none = {NULL, NULL};

    if (!name)
        return none;

    for (size_t i = 0; i < functions_count; i++)
        if (!strcmp(functions_list[i].name, name))
            return functions_list[i];

    return none;
}

////////////////////////////////////////////////////////////
char *soare_getarg(soare_arguments_list args, unsigned int position)
{
    for (; position && args; position--)
        args = args->sibling;
    return Math(args);
}

/*

==============================================================
Example: Custom Keyword - Clear screen
==============================================================

This example demonstrates how to implement a custom keyword.

----------------------------------------------------------
Function Name:
    void clear(void)

----------------------------------------------------------
Code:
----------------------------------------------------------

void clear(void)
{
    // ANSI escape code to clear the screen
    printf("\033c\033[3J");
}

Implement this keyword: soare_addkeyword(<keyword name>, <function>)

soare_addkeyword("clear", clear);

----------------------------------------------------------

*/

/* Keywords */
static struct soare_keywords keywords_list[100];
/* Count */
static unsigned long keywords_count = 0;

////////////////////////////////////////////////////////////
unsigned long soare_addkeyword(char *name, void (*keyword)(void))
{
    if (keywords_count >= 99 || !name || !keyword)
        return 0;

    struct soare_keywords fn = {name, keyword};

    keywords_list[keywords_count] = fn;
    keywords_list[keywords_count + 1].name = NULL;

    keywords_count = keywords_count + 1;
    return keywords_count;
}

////////////////////////////////////////////////////////////
soare_keyword soare_getkeyword(char *name)
{
    static soare_keyword none = {NULL, NULL};

    if (!name)
        return none;

    for (size_t i = 0; i < keywords_count; i++)
        if (!strcmp(keywords_list[i].name, name))
            return keywords_list[i];

    return none;
}

////////////////////////////////////////////////////////////
bBool soare_is_custom_keyword(char *name)
{
    if (!name)
        return bFalse;

    for (size_t i = 0; i < keywords_count; i++)
        if (!strcmp(keywords_list[i].name, name))
            return bTrue;

    return bFalse;
}
