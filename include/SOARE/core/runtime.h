#ifndef __SOARE_RUNTIME_H__
#define __SOARE_RUNTIME_H__

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
 * @brief Execute a function represented by an AST node
 *
 * @param tree AST node representing the function to run
 * @return char* Allocated result string, or NULL on error
 */
char *soare_run_function(ast_t tree);

/**
 * @brief Execute SOARE source code
 *
 * @param file Logical filename associated
 * @param rawcode Source code buffer to execute
 * @return char* Result string
 */
char *soare_execute(char *__restrict__ filename, char *__restrict__ rawcode);

/**
 * @brief Free resources used by the interpreter
 */
void soare_kill(void);

#endif /* __SOARE_RUNTIME_H__ */