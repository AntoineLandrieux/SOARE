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
 * Antoine LANDRIEUX (MIT License) <Error.c>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

#include <SOARE/SOARE.h>

/* Enable/disable error display */
static unsigned char enable = 1;

/* Error level */
static char errorlevel = 0;

/* Exceptions */
static char *Exceptions[] = {

    "InterpreterError",
    "FileError",
    "CharacterError",
    "SyntaxError",
    "ValueError",
    "UnexpectedNear",
    "UndefinedReference",
    "ObjectIsNotCallable",
    "VariableDefinedAsFunction",
    "MathError",
    "InvalidEscapeSequence",
    "IndexOutOfRange",
    "DivideByZero",
    "RaiseException"

};

/**
 * @brief Error display
 * @author Antoine LANDRIEUX
 *
 * @return unsigned char
 */
unsigned char AsIgnoredException(void)
{
    return !enable;
}

/**
 * @brief Enable/disable error display
 * @author Antoine LANDRIEUX
 *
 * @param ignore
 */
void IgnoreException(unsigned char ignore)
{
    enable = !ignore;
}

/**
 * @brief Clears errors
 * @author Antoine LANDRIEUX
 *
 */
void ClearException(void)
{
    errorlevel = 0;
}

/**
 * @brief Returns the error level
 * @author Antoine LANDRIEUX
 *
 * @return char
 */
char ErrorLevel(void)
{
    return errorlevel;
}

/**
 * @brief Create a new error, and display it
 * @author Antoine LANDRIEUX
 *
 * @param error
 * @param string
 * @param file
 * @return void* (always returns NULL)
 */
void *LeaveException(SoareExceptions error, char *string, Document file)
{
    // If the errors are disabled, nothing is displayed
    if (enable)
    {
#ifndef __SOARE_NO_COLORED_OUTPUT
        // Red
        fprintf(stderr, "\033[31m");
#endif /* __SOARE_NO_COLORED_OUTPUT */
        fprintf(
            //
            stderr,
            "Except: %s\n\t\"%.13s\"\n\t ^~~~\n\tAt file %s:%lld:%lld\n",
            Exceptions[error],
            string,
            file.file,
            file.ln,
            file.col
            //
        );
#ifndef __SOARE_NO_COLORED_OUTPUT
        // Normal
        fprintf(stderr, "\033[0m");
#endif /* __SOARE_NO_COLORED_OUTPUT */
    }
    // Set error at level 1
    errorlevel = 1;
    // Store the error in the variable
    MemSet(MemGet(MEMORY, "__ERROR__"), strdup(Exceptions[error]));
    return NULL;
}
