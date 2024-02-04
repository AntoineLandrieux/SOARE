#ifndef __SAE_UTILS_H__
#define __SAE_UTILS_H__ 0x1

#define char_IsNaN(_Char) (!(_Char >= '0' && _Char <= '9'))
#define char_Alpha(_Char) ((_Char >= 'a' && _Char <= 'z') || (_Char >= 'A' && _Char <= 'Z'))
#define char_AlNum(_Char) (char_Alpha(_Char) || !char_IsNaN(_Char))
#define char_Quote(_Char) (_Char == '`' || _Char == '\'' || _Char == '\"')
#define char_Bckln(_Char) (_Char == '\n' || _Char == '\r')
#define char_Space(_Char) (char_Bckln(_Char) || _Char == '\t' || _Char == ' ')

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Char
 * @return unsigned char
 */
unsigned char char_operator(const char _Char);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Op
 * @return unsigned char
 */
unsigned char char_mathPriority(const char _Op);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _String
 * @return unsigned char
 */
unsigned char string_isNaN(const char *_String);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _String
 * @return unsigned char
 */
unsigned char string_keyword(const char *_String);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _String
 * @param _To
 * @return char*
 */
char *string_slice(const char *_String, const unsigned long long _To);

/**
 * @author Antoine LANDRIEUX
 *
 * @param x
 * @param y
 * @return char*
 */
char *string_eval(char *x, char operator, char * y);

/**
 * @author Antoine LANDRIEUX
 *
 * @param _Float
 * @return int
 */
int float_count(double _Float);

#endif /* __SAE_UTILS_H__ */