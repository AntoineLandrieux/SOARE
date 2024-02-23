#ifndef __SAE_ERROR_H__
#define __SAE_ERROR_H__ 0x1

/**
 * @brief SOARE errorlang
 * @author Antoine LANDRIEUX
 */
typedef enum errorlang
{
    ERROR_FILE,
    ERROR_CHARACTER,
    ERROR_TOKEN,
    ERROR_SYNTAX,
    ERROR_EXPRESSION
} ErrorLang;

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Err
 * @param _Extra
 * @param _Ln
 * @param _Col
 * @return void*
 */
void *ThrowsLang(ErrorLang _Err, const char *_Extra, unsigned long long _Ln, unsigned long long _Col);

#endif /* __SAE_ERROR_H__ */