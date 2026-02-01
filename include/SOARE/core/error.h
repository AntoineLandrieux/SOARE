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
 * @enum soare_exceptions
 * @brief Enumerates the different exception types used by the interpreter.
 */
typedef enum soare_exceptions
{

    InterpreterError,          /**< Generic interpreter error (fatal)       */
    FileError,                 /**< File I/O related error                  */
    CharacterError,            /**< Invalid character encountered           */
    SyntaxError,               /**< Parsing / syntax error                  */
    ValueError,                /**< Invalid value or type                   */
    UnexpectedNear,            /**< Unexpected token near ...               */
    UndefinedReference,        /**< Name is not defined                     */
    MissingArgument,           /**< Function or keyword missing an argument */
    ObjectIsNotCallable,       /**< Attempt to call a non-callable object   */
    VariableDefinedAsFunction, /**< Variable name conflicts with function   */
    AssignConstantVariable,    /**< Assignment to constant variable         */
    MathError,                 /**< Generic math error (overflow, etc.)     */
    InvalidEscapeSequence,     /**< Bad string escape                       */
    IndexOutOfRange,           /**< Indexing outside container bounds       */
    DivideByZero,              /**< Division by zero                        */
    RaiseException             /**< Explicitly raised exception             */

} soare_exceptions_t;

/**
 * @brief Retrieve the last formatted exception message.
 *
 * @return Pointer to a null-terminated string describing the last error,
 *         or NULL if no error is stored.
 */
char *soare_get_exception(void);

/**
 * @brief Clear errorlevel.
 *
 */
void soare_clear_exception(void);

/**
 * @brief Enable or disable displaying exceptions to the user.
 *
 * @param ignore Non-zero to suppress error display, zero to enable display.
 */
void soare_ignore_exception(boolean_t ignore);

/**
 * @brief Query whether exceptions are currently ignored.
 *
 * @return boolean_t non-zero when exceptions are suppressed, zero otherwise.
 */
boolean_t soare_as_ignored_exception(void);

/**
 * @brief Get the current interpreter error level.
 *
 * @return An integer error level.
 */
int soare_errorlevel(void);

/**
 * @brief Create and display a new exception.
 *
 * This function records an exception of the given type and associates a
 * human-readable message and source `document_t` (file/position) with it.
 * It returns NULL to allow usage patterns like `return soare_leave_exception(...);`.
 *
 * @param error    Exception code from `soare_exceptions_t`.
 * @param string   Descriptive message (format already applied).
 * @param file     Document context where the error occurred.
 * @return void*   Always returns NULL.
 */
void *soare_leave_exception(soare_exceptions_t error, const char *string, document_t file);

/**
 * @brief Shorthand macro for out-of-memory failures.
 */
#define __SOARE_OUT_OF_MEMORY() soare_leave_exception(InterpreterError, "OUT OF MEMORY", soare_empty_document())

#endif /* __SOARE_ERROR_H__ */