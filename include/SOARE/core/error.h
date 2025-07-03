#ifndef __SOARE_ERROR_H__
#define __SOARE_ERROR_H__ 0x1

/* #pragma once */

/**
 *  _____  _____  ___  ______ _____
 * /  ___||  _  |/ _ \ | ___ \  ___|
 * \ `--. | | | / /_\ \| |_/ / |__
 *  `--. \| | | |  _  ||    /|  __|
 * /\__/ /\ \_/ / | | || |\ \| |___
 * \____/  \___/\_| |_/\_| \_\____/
 *
 * Antoine LANDRIEUX (MIT License) <error.h>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

/**
 * @brief List the different types of errors
 */
typedef enum SoareExceptions
{

    InterpreterError,
    FileError,
    CharacterError,
    SyntaxError,
    ValueError,
    UnexpectedNear,
    UndefinedReference,
    ObjectIsNotCallable,
    VariableDefinedAsFunction,
    MathError,
    InvalidEscapeSequence,
    IndexOutOfRange,
    DivideByZero,
    RaiseException

} SoareExceptions;

/**
 * @brief Error display
 *
 * @return unsigned char
 */
unsigned char AsIgnoredException(void);

/**
 * @brief Enable/disable error display
 *
 * @param ignore
 */
void IgnoreException(unsigned char ignore);

/**
 * @brief Clears errors
 *
 */
void ClearException(void);

/**
 * @brief Returns the error level
 *
 * @return char
 */
char ErrorLevel(void);

/**
 * @brief Create a new error, and display it
 *
 * @param error
 * @param string
 * @param file
 * @return void* (always returns NULL)
 */
void *LeaveException(SoareExceptions error, char *string, Document file);

/* Alias when memory allocation failed. Out of memory */
#define __SOARE_OUT_OF_MEMORY() LeaveException(InterpreterError, "OUT OF MEMORY", EmptyDocument())

#endif /* __SOARE_ERROR_H__ */