#include <stdio.h>
#include <stdlib.h>

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
static u8 enable = 1;

/* Error level */
static i8 ErrorLvl = 0;

/* Exceptions */
static char *Exceptions[] = {

    "InterpreterError",
    "FileError",
    "CharacterError",
    "SyntaxError",
    "UnexpectedNear",
    "UndefinedReference",
    "ObjectIsNotCallable",
    "VariableDefinedAsFunction",
    "MathError",
    "IndexOutOfRange",
    "DivideByZero",
    "RaiseException"

};

/**
 * @brief Error display
 * @author Antoine LANDRIEUX
 *
 */
u8 AsIgnoredException()
{
    return !enable;
}

/**
 * @brief Enable/disable error display
 * @author Antoine LANDRIEUX
 *
 * @param ignore
 */
void IgnoreException(u8 ignore)
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
    ErrorLvl = 0;
}

/**
 * @brief Returns the error level
 * @author Antoine LANDRIEUX
 *
 * @return char
 */
char ErrorLevel(void)
{
    return ErrorLvl;
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
        fprintf(stderr, "\033[0;39m");
#endif /* __SOARE_NO_COLORED_OUTPUT */
    }
    // set error at level 1
    ErrorLvl = 1;
    return NULL;
}
