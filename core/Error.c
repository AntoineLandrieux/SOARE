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
static bBool enable = bTrue;

/* Last Error */
static char *lasterror = "NoError";

/* Error level */
static int errorlevel = EXIT_SUCCESS;

/* Exceptions */
static char *Exceptions[] = {

    "InterpreterError",
    "FileError",
    "CharacterError",
    "SyntaxError",
    "ValueError",
    "UnexpectedNear",
    "UndefinedReference",
    "MissingArgument",
    "ObjectIsNotCallable",
    "VariableDefinedAsFunction",
    "MathError",
    "InvalidEscapeSequence",
    "IndexOutOfRange",
    "DivideByZero",
    "RaiseException"

};

////////////////////////////////////////////////////////////
char *soare_get_exception(void)
{
    return lasterror;
}

////////////////////////////////////////////////////////////
void soare_clear_exception(void)
{
    errorlevel = EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////
void soare_ignore_exception(bBool ignore)
{
    enable = !ignore;
}

////////////////////////////////////////////////////////////
bBool soare_as_ignored_exception(void)
{
    return !enable;
}

////////////////////////////////////////////////////////////
int soare_errorlevel(void)
{
    return errorlevel;
}

////////////////////////////////////////////////////////////
void *LeaveException(SoareExceptions error, const char *string, Document file)
{
    // Set lasterror
    lasterror = Exceptions[error];
    // Set error at level EXIT_FAILURE (1)
    errorlevel = EXIT_FAILURE;

    // If the errors are disabled, nothing is displayed
    if (enable)
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
            lasterror,
            string,
            file.file,
            file.ln,
            file.col
            //
        );
    }

    return NULL;
}
