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
 * @brief Parse an expression with the given operator precedence priority.
 *
 * This function implements expression parsing using precedence climbing or
 * a similar algorithm. `priority` controls the minimum operator precedence
 * level that this call will parse; higher-level calls can use it for
 * recursive parsing of sub-expressions.
 *
 * @param tokens Pointer to the token stream pointer. The function advances
 *               the pointer as tokens are consumed.
 * @param priority Minimum operator precedence to parse at this level.
 * @return ast_t An AST representing the parsed expression subtree, or NULL on error.
 */
ast_t soare_parse_expression(tokens_t **tokens, short priority);

/**
 * @brief Evaluate the given AST containing a mathematical expression.
 *
 * The evaluator traverses the AST and computes a string representation of
 * the numerical result according to SOARE runtime conventions. The returned
 * string is typically heap-allocated and should be freed by the caller when
 * no longer needed (see project conventions).
 *
 * @param tree AST to evaluate.
 * @return char* Newly allocated string representing the result, or NULL on error.
 */
char *soare_math(ast_t tree);

#endif /* __SOARE_MATH_H__ */