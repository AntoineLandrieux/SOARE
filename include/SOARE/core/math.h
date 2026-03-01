#ifndef __SOARE_MATH_H__
#define __SOARE_MATH_H__

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
 * @brief Parse an expression with the given operator precedence priority
 *
 * @param tokens Pointer to the token stream pointer
 * @param priority Minimum operator precedence to parse at this level
 * @return ast_t An AST representing the parsed expression subtree, or NULL on error
 */
ast_t soare_parse_expression(tokens_t **tokens, short priority);

/**
 * @brief Evaluate the given AST containing a mathematical expression
 *
 * @param tree AST to evaluate
 * @return char* Allocated string representing the result, or NULL on error
 */
char *soare_math(ast_t tree);

#endif /* __SOARE_MATH_H__ */