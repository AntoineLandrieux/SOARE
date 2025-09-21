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
static unsigned char enable = 1;

/* Error level */
static char errorlevel = EXIT_SUCCESS;

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
 *
 * @return unsigned char
 */
unsigned char AsIgnoredException(void)
{
    return !enable;
}

/**
 * @brief Enable/disable error display
 *
 * @param ignore
 */
void IgnoreException(unsigned char ignore)
{
    enable = !ignore;
}

/**
 * @brief Clears errors
 *
 */
void ClearException(void)
{
    errorlevel = EXIT_SUCCESS;
}

/**
 * @brief Returns the error level
 *
 * @return char
 */
char ErrorLevel(void)
{
    return errorlevel;
}

/**
 * @brief Create a new error, and display it
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
#ifdef __SOARE_COLORED_OUTPUT
        // Red
        soare_write(__soare_stderr, "\033[91m");
#endif /* __SOARE_COLORED_OUTPUT */

        soare_write(
            //
            __soare_stderr,
            "\aExcept: %s\n\t\"%.13s\"\n\t ^~~~\n\tAt file %s:%lld:%lld\n",
            Exceptions[error],
            string,
            file.file,
            file.ln,
            file.col
            //
        );

#ifdef __SOARE_COLORED_OUTPUT
        // Normal
        soare_write(__soare_stderr, "\033[0m");
#endif /* __SOARE_COLORED_OUTPUT */
    }

    // Set error at level EXIT_FAILURE (1)
    errorlevel = EXIT_FAILURE;
    return NULL;
}
