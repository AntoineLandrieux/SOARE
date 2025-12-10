#ifndef __SOARE_CUSTOM_H__
#define __SOARE_CUSTOM_H__ 0x1

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
 * ////////////////////////////////////////////////////////////
 *  FUNCTION
 * ////////////////////////////////////////////////////////////
 */

/**
 * @brief Soare defined function
 */
typedef struct soare_functions
{

    char *name;
    char *(*exec)(soare_arguments_list);

} soare_function;

/**
 * @brief Add defined function
 *
 * @param name
 * @param function
 * @return unsigned long
 */
unsigned long soare_addfunction(char *name, char *(*function)(soare_arguments_list));

/**
 * @brief Get defined function
 *
 * @param name
 * @return soare_function
 */
soare_function soare_getfunction(char *name);

/**
 * @brief Get argument from a function call
 *
 * @param args
 * @param position
 * @return char*
 */
char *soare_getarg(soare_arguments_list args, unsigned int position);

/**
 * ////////////////////////////////////////////////////////////
 *  KEYWORDS
 * ////////////////////////////////////////////////////////////
 */

/**
 * @brief Soare defined keyword
 */
typedef struct soare_keywords
{

    char *name;
    void (*exec)(void);

} soare_keyword;

/**
 * @brief Add defined keyword
 *
 * @param name
 * @param keyword
 * @return unsigned long
 */
unsigned long soare_addkeyword(char *name, void (*keyword)(void));

/**
 * @brief Get defined keyword
 *
 * @param name
 * @return soare_keyword
 */
soare_keyword soare_getkeyword(char *name);

/**
 * @brief Check if a keyword exists
 *
 * @param name
 * @return bBool
 */
bBool soare_is_custom_keyword(char *name);

#endif /* __SOARE_CUSTOM_H__ */