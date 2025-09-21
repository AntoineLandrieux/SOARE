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
 * @brief Initialize SOARE interpreter
 * 
 */
void soare_init(void);

/**
 * @brief Kill SOARE interpreter
 * 
 */
void soare_kill(void);

/**
 * @brief Execute a function
 *
 * @param tree
 * @return char*
 */
char *RunFunction(AST tree);

/**
 * @brief Execute SOARE code
 *
 * @param file
 * @param rawcode
 * @return char *
 */
char *Execute(char *__restrict__ file, char *__restrict__ rawcode);

#endif /* __SOARE_RUNTIME_H__ */