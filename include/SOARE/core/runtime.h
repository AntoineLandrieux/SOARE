#ifndef __SOARE_RUNTIME_H__
#define __SOARE_RUNTIME_H__ 0x1

/* #pragma once */

/**
 *  _____  _____  ___  ______ _____
 * /  ___||  _  |/ _ \ | ___ \  ___|
 * \ `--. | | | / /_\ \| |_/ / |__
 *  `--. \| | | |  _  ||    /|  __|
 * /\__/ /\ \_/ / | | || |\ \| |___
 * \____/  \___/\_| |_/\_| \_\____/
 *
 * Antoine LANDRIEUX (MIT License) <runtime.h>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

/**
 * @brief Execute a function represented by an AST node.
 *
 * This evaluates the AST corresponding to a function body and returns the
 * resulting string value according to runtime conventions. The returned
 * pointer is typically owned by the caller and should be freed when no
 * longer needed.
 *
 * @param tree AST node representing the function to run.
 * @return char* Newly allocated result string, or NULL on error.
 */
char *soare_run_function(ast_t tree);

/**
 * @brief Execute SOARE source code.
 *
 * Parses and executes the given `rawcode` using `file` as the logical
 * filename for diagnostics. Returns the string result of the execution or
 * NULL on error.
 *
 * @param file Logical filename associated with `rawcode` (for errors).
 * @param rawcode Source code buffer to execute.
 * @return char* Result string (caller-owned) or NULL on failure.
 */
char *soare_execute(char *__restrict__ filename, char *__restrict__ rawcode);

/**
 * @brief Shutdown and free resources used by the interpreter.
 *
 * After calling this function, no further SOARE API calls should be made
 * until `soare_init()` is called again. It releases global state and
 * performs cleanup of allocated resources.
 */
void soare_kill(void);

#endif /* __SOARE_RUNTIME_H__ */