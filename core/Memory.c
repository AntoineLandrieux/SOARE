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
 * Antoine LANDRIEUX (MIT License) <Memory.c>
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
    char *int_add(soare_arguments_list_t args)

Arguments:
    - args: A linked list of arguments passed to the function.
        Use soare_get_argument(args, i) to retrieve the i-th argument
        as a string.

Return Value:
    - Returns NULL (no value returned to SOARE).
    - If you want to return a value, allocate memory for the
        result string before returning.

----------------------------------------------------------
Implementation Steps:
    1. Initialize result accumulator.
    2. Loop through arguments using soare_get_argument.
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

char *int_add(soare_arguments_list_t args)
{
    // Accumulator for the sum
    int result = 0;
    // Pointer to current argument string
    char *x = NULL;

    // Loop through all arguments
    for (unsigned int i = 0; 1; i++)
    {
        // Retrieve the i-th argument
        x = soare_get_argument(args, i);
        if (!x)
            // Exit loop if no more arguments
            break;

        // Convert argument to integer and add to result
        result += atoi(x);
        // Free the memory used by x
        free(x);
    }

    // Output the result to the console
    printf("%d", result);

    // Return NULL (no value returned to SOARE)
    // If you need to return a value, allocate memory as shown above
    return NULL;
}

Implement this function: soare_add_function(<function name>, <function>)

soare_add_function("int_add", int_add);

----------------------------------------------------------

*/

/* Functions */
static soare_functions_t *functions_list = NULL;
static soare_functions_t *functions_list_ptr = NULL;

////////////////////////////////////////////////////////////
soare_functions_t *soare_add_function(char *name, char *(*function)(soare_arguments_list_t))
{
    if (!name || !function)
        return NULL;

    soare_functions_t *node = (soare_functions_t *)malloc(sizeof(soare_functions_t));

    if (!node)
        return __SOARE_OUT_OF_MEMORY();

    node->name = name;
    node->exec = function;
    node->next = NULL;

    if (!functions_list)
    {
        functions_list = node;
        functions_list_ptr = node;
        return node;
    }

    functions_list_ptr->next = node;
    functions_list_ptr = node;

    return node;
}

////////////////////////////////////////////////////////////
soare_functions_t *soare_get_function(char *name)
{
    for (soare_functions_t *function = functions_list; function; function = function->next)
        if (!strcmp(function->name, name))
            return function;
    return NULL;
}

////////////////////////////////////////////////////////////
void soare_clear_functions(void)
{
    soare_functions_t *list = functions_list;

    while (list)
    {
        soare_functions_t *next = list->next;
        free(list);
        list = next;
    }

    functions_list = NULL;
    functions_list_ptr = NULL;
}

////////////////////////////////////////////////////////////
char *soare_get_argument(soare_arguments_list_t args, unsigned int position)
{
    for (; position && args; position--)
        args = args->sibling;
    return soare_math(args);
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

Implement this keyword: soare_add_keyword(<keyword name>, <function>)

soare_add_keyword("clear", clear);

----------------------------------------------------------

*/

/* Keywords */
static soare_keywords_t *keywords_list = NULL;
static soare_keywords_t *keywords_list_ptr = NULL;

////////////////////////////////////////////////////////////
soare_keywords_t *soare_add_keyword(char *name, void (*keyword)(void))
{
    if (!name || !keyword)
        return NULL;

    soare_keywords_t *node = (soare_keywords_t *)malloc(sizeof(soare_keywords_t));

    if (!node)
        return __SOARE_OUT_OF_MEMORY();

    keywords_list_ptr->name = name;
    keywords_list_ptr->exec = keyword;
    keywords_list_ptr->next = NULL;

    if (!keywords_list)
    {
        keywords_list = node;
        keywords_list_ptr = node;
        return node;
    }

    keywords_list_ptr->next = node;
    keywords_list_ptr = node;

    return node;
}

////////////////////////////////////////////////////////////
soare_keywords_t *soare_get_keyword(char *name)
{
    for (soare_keywords_t *keyword = keywords_list; keyword; keyword = keyword->next)
        if (!strcmp(keyword->name, name))
            return keyword;
    return NULL;
}

////////////////////////////////////////////////////////////
void soare_clear_keywords(void)
{
    soare_keywords_t *list = keywords_list;

    while (list)
    {
        soare_keywords_t *next = list->next;
        free(list);
        list = next;
    }

    keywords_list = NULL;
    keywords_list_ptr = NULL;
}

/*

==============================================================
Example: Custom Variable - Boolean
==============================================================

This example demonstrates how to implement a custom variable.

----------------------------------------------------------
Code:
----------------------------------------------------------

char *bool_true  = "1";
char *bool_false = "0";

Implement these variables: soare_add_keyword(<variable name>, <value>, <mutable>)

soare_add_variable("true",  bool_true,  0);
soare_add_variable("false", bool_false, 0);

----------------------------------------------------------

*/

/* Variables */
static soare_variables_t *variables_list = NULL;
static soare_variables_t *variables_list_ptr = NULL;

////////////////////////////////////////////////////////////
soare_variables_t *soare_add_variable(char *name, char *value, boolean_t mutable)
{
    if (!name)
        return NULL;

    soare_variables_t *node = (soare_variables_t *)malloc(sizeof(soare_variables_t));

    if (!node)
        return __SOARE_OUT_OF_MEMORY();

    node->name = name;
    node->value = value ? strdup(value) : NULL;
    node->mutable = mutable;
    node->scope = 0;
    node->body = NULL;
    node->prev = NULL;
    node->next = NULL;

    if (!variables_list)
    {
        variables_list = node;
        variables_list_ptr = node;
        return node;
    }

    node->prev = variables_list_ptr;
    variables_list_ptr->next = node;
    variables_list_ptr = node;

    return node;
}

////////////////////////////////////////////////////////////
soare_variables_t *soare_get_variable(char *name)
{
    for (soare_variables_t *var = variables_list_ptr; var; var = var->prev)
        if (!strcmp(var->name, name))
            return var;
    return NULL;
}

////////////////////////////////////////////////////////////
void soare_clear_variables(void)
{
    soare_variables_t *list = variables_list;

    while (list)
    {
        soare_variables_t *next = list->next;
        free(list->value);
        free(list);
        list = next;
    }

    variables_list = NULL;
    variables_list_ptr = NULL;
}

////////////////////////////////////////////////////////////
void soare_clear_variables_scope(unsigned long long scope)
{
    soare_variables_t *list = variables_list_ptr;

    while (list)
    {
        soare_variables_t *prev = list->prev;
        list->next = NULL;

        if (list->scope < scope)
        {
            variables_list_ptr = list;
            return;
        }

        free(list->value);
        free(list);
        list = prev;
    }

    variables_list = NULL;
    variables_list_ptr = NULL;
}
