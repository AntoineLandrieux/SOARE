#ifndef __SOARE_MATH_H__
#define __SOARE_MATH_H__ 0x1

/* #pragma once */

/**
 *  _____  _____  ___  ______ _____
 * /  ___||  _  |/ _ \ | ___ \  ___|
 * \ `--. | | | / /_\ \| |_/ / |__
 *  `--. \| | | |  _  ||    /|  __|
 * /\__/ /\ \_/ / | | || |\ \| |___
 * \____/  \___/\_| |_/\_| \_\____/
 *
 * Antoine LANDRIEUX (MIT License) <math.h>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

/**
 * @brief Return the value as a node
 *
 * @param tokens
 * @return AST
 */
AST ParseValue(Tokens **tokens);

/**
 * @brief Build a math tree
 *
 * @param tokens
 * @param priority
 * @return AST
 */
AST ParseExpr(Tokens **tokens, unsigned char priority);

/**
 * @brief Get the Array Index object
 *
 * @param array
 * @param size
 * @return long long
 */
long long GetArrayIndex(AST array, size_t size);

/**
 * @brief Evaluates the mathematical expression of a tree
 *
 * @param tree
 * @return char*
 */
char *Eval(AST tree);

#endif /* __SOARE_MATH_H__ */