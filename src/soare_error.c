#include <stdio.h>
#include <stdlib.h>

#include <SOARE/SAE/sae_error.h>

static const char *ERRORS[] = {
    "Can't read file",
    "Unexpected character",
    "Unexpected token",
    "Invalid syntax",
    "Invalid expression"
};

void *ThrowsLang(ErrorLang _Err, const char *_Extra, unsigned long long _Ln, unsigned long long _Col)
{
    fprintf(stderr,
            "ERROR: %s\nAt \"%s\"\n~~~~^\nAt file:%lld:%lld\n",
            ERRORS[_Err], _Extra, _Ln, _Col);
    return NULL;
}
