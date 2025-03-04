#ifndef __SOARE_ERROR_H__
#define __SOARE_ERROR_H__ 0x1

/* #prama once */

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
 * @author Antoine LANDRIEUX
 */
typedef enum SoareExceptions
{

    InterpreterError,
    FileError,
    CharacterError,
    SyntaxError,
    UnexpectedNear,
    UndefinedReference,
    MathError,
    IndexOutOfRange,
    DivideByZero,
    RaiseException

} SoareExceptions;

/**
 * @brief Error display
 * @author Antoine LANDRIEUX
 *
 */
u8 AsIgnoredException();

/**
 * @brief Enable/disable error display
 * @author Antoine LANDRIEUX
 *
 * @param ignore
 */
void IgnoreException(u8 ignore);

/**
 * @brief Clears errors
 * @author Antoine LANDRIEUX
 *
 */
void ClearException(void);

/**
 * @brief Returns the error level
 * @author Antoine LANDRIEUX
 *
 * @return char
 */
char ErrorLevel(void);

/**
 * @brief Create a new error, and display it
 * @author Antoine LANDRIEUX
 *
 * @param error
 * @param string
 * @param file
 * @return void* (always returns NULL)
 */
void *LeaveException(SoareExceptions error, char *string, Document file);

#define __SOARE_OUT_OF_MEMORY() LeaveException(InterpreterError, "OUT OF MEMORY", EmptyDocument())

#endif /* __SOARE_ERROR_H__ */