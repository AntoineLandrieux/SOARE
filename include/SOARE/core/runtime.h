#ifndef __SOARE_RUNTIME_H__
#define __SOARE_RUNTIME_H__ 0x1

/* #prama once */

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

// Memory used by the interpreter
extern MEM MEMORY;

/**
 * @brief Execute a function
 * @author Antoine LANDRIEUX
 *
 * @deprecated
 *
 * @param tree
 * @return char*
 */
char *RunFunction(AST tree);

/**
 * @brief Executes code from a tree
 * @author Antoine LANDRIEUX
 *
 * @param tree
 * @return char*
 */
char *Runtime(AST tree);

/**
 * @brief Execute the code from a string
 * @author Antoine LANDRIEUX
 *
 * @param rawcode
 */
int Execute(char *file, char *rawcode);

#endif /* __SOARE_RUNTIME_H__ */