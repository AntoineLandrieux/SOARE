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

/* Enable/Disable error display */
static boolean_t enable_error_display = bTrue;

/* Contains the current SOARE error level */
static int error_level = EXIT_SUCCESS;

/* Contains the latest SOARE error */
static char *last_error = NULL;

/* List of SOARE Exceptions */
static char *exceptions_list[] = {

    "InterpreterError",
    "MemoryError",
    "FileError",
    "CharacterError",
    "SyntaxError",
    "ValueError",
    "UnexpectedNear",
    "UndefinedReference",
    "MissingArgument",
    "ObjectIsNotCallable",
    "VariableDefinedAsFunction",
    "AssignConstantVariable",
    "MathError",
    "InvalidEscapeSequence",
    "IndexOutOfRange",
    "DivideByZero",
    "RaiseException"

};

////////////////////////////////////////////////////////////
char *soare_get_exception(void)
{
    return last_error;
}

////////////////////////////////////////////////////////////
void soare_clear_exception(void)
{
    error_level = EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////
void soare_ignore_exception(boolean_t ignore)
{
    enable_error_display = !ignore;
}

////////////////////////////////////////////////////////////
boolean_t soare_as_ignored_exception(void)
{
    return !enable_error_display;
}

////////////////////////////////////////////////////////////
int soare_errorlevel(void)
{
    return error_level;
}

////////////////////////////////////////////////////////////
void soare_leave_exception(soare_exceptions_t error, const char *string, document_t file)
{
    // Set lasterror
    last_error = exceptions_list[error];
    // Set error at level EXIT_FAILURE (1)
    error_level = EXIT_FAILURE;

    // If the errors are disabled, nothing is displayed
    if (enable_error_display)
    {
        soare_write(
            //
            __soare_stderr,
#ifdef __SOARE_COLORED_OUTPUT
            // Red
            "\033[91m"
#endif /* __SOARE_COLORED_OUTPUT */
            "\aExcept: %s\n\t\"%.13s\"\n\t ^~~~\n\tAt file %s:%lld:%lld\n"
#ifdef __SOARE_COLORED_OUTPUT
            // Normal
            "\033[0m"
#endif /* __SOARE_COLORED_OUTPUT */
            ,
            last_error,
            string,
            file.filename,
            file.ln,
            file.col
            //
        );
    }
}
