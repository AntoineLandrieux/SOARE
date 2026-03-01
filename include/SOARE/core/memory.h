#ifndef __SOARE_MEMORY_H__
#define __SOARE_MEMORY_H__

/* #pragma once */

/**
 *  _____  _____  ___  ______ _____
 * /  ___||  _  |/ _ \ | ___ \  ___|
 * \ `--. | | | / /_\ \| |_/ / |__
 *  `--. \| | | |  _  ||    /|  __|
 * /\__/ /\ \_/ / | | || |\ \| |___
 * \____/  \___/\_| |_/\_| \_\____/
 *
 * Antoine LANDRIEUX (MIT License) <custom.h>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

/**
 * @typedef soare_arguments_list_t
 * @brief Alias for `ast_t` representing a function arguments list
 */
typedef ast_t soare_arguments_list_t;

/**
 * @brief Representation of a SOARE-defined function
 */
typedef struct soare_functions
{

    char *name;                            /**< Function name            */
    char *(*exec)(soare_arguments_list_t); /**< Function implementation  */
    struct soare_functions *next;          /**< Next registered function */

} soare_functions_t;

/**
 * @brief Register a new function
 *
 * @param name Null-terminated string identifier for the function
 * @param function Pointer to the implementation to be invoked
 * @return soare_functions_t* Registered function
 */
soare_functions_t *soare_add_function(char *name, char *(*function)(soare_arguments_list_t));

/**
 * @brief Find a registered function by name
 *
 * @param name Function name to look up
 * @return soare_functions_t* Pointer to the function entry, or NULL if not found
 */
soare_functions_t *soare_get_function(char *name);

/**
 * @brief Remove and free all registered functions
 */
void soare_clear_functions(void);

/**
 * @brief Retrieve an argument from a function call
 *
 * @param args Argument list provided to the function
 * @param position Zero-based index of the requested argument
 * @return char* Pointer to the argument string, or NULL if out of range
 */
char *soare_get_argument(soare_arguments_list_t args, unsigned int position);

/**
 * @brief Representation of a SOARE-defined keyword
 */
typedef struct soare_keywords
{

    char *name;                  /**< Keyword name                      */
    void (*exec)(void);          /**< Callback executed for the keyword */
    struct soare_keywords *next; /**< Next registered keyword           */

} soare_keywords_t;

/**
 * @brief Register a new keyword
 *
 * @param name Null-terminated keyword identifier
 * @param keyword Callback invoked when the keyword is used
 * @return soare_keywords_t Registered keyword
 */
soare_keywords_t *soare_add_keyword(char *name, void (*keyword)(void));

/**
 * @brief Find a registered keyword by name
 *
 * @param name Keyword name to look up
 * @return soare_keywords_t* Pointer to the keyword entry, or NULL if not found
 */
soare_keywords_t *soare_get_keyword(char *name);

/**
 * @brief Remove and free all registered keywords
 */
void soare_clear_keywords(void);

/**
 * @brief Representation of a SOARE-defined variable
 */
typedef struct soare_variables
{

    char *name;                   /**< Identifier name                  */
    char *value;                  /**< Variable value string            */
    ast_t body;                   /**< Function body AST                */
    boolean_t mutable;            /**< Variable is mutable              */
    unsigned long long scope;     /**< Scope index                      */
    struct soare_variables *prev; /**< Previous memory node in the list */
    struct soare_variables *next; /**< Next memory node in the list     */

} soare_variables_t;

/**
 * @brief Register a new variable
 *
 * @param name Null-terminated variable identifier
 * @param value Null-terminated variable value
 * @param mutable Variable is mutable
 * @return soare_variables_t Registered variable
 */
soare_variables_t *soare_add_variable(char *name, char *value, boolean_t mutable);

/**
 * @brief Find a registered keyword by name
 *
 * @param name Keyword name to look up
 * @return soare_keywords* Pointer to the keyword entry, or NULL if not found
 */
soare_variables_t *soare_get_variable(char *name);

/**
 * @brief Increment the current scope level
 */
void soare_up_scope(void);

/**
 * @brief Decrement the current scope level
 */
void soare_down_scope(void);

/**
 * @brief Remove all variables in the current scope
 */
void soare_clear_scope(void);

/**
 * @brief Remove and free all registered variables
 */
void soare_clear_variables(void);

#endif /* __SOARE_MEMORY_H__ */