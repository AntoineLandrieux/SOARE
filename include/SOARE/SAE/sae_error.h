#ifndef __SAE_ERROR_H__
#define __SAE_ERROR_H__ 0x1

/*
 _____  _____  ___  ______ _____
/  ___||  _  |/ _ \ | ___ \  ___|
\ `--. | | | / /_\ \| |_/ / |__
 `--. \| | | |  _  ||    /|  __|
/\__/ /\ \_/ / | | || |\ \| |___
\____/  \___/\_| |_/\_| \_\____/

 * Antoine LANDRIEUX
 * GNU General Public License v3.0
 * https://www.gnu.org/licenses/
 * 
 * SOARE HEADER
 * sae_error.h
*/

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