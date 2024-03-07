#include <stdio.h>
#include <stdlib.h>

#include <SOARE/SAE/sae_error.h>

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
 * SOARE
 * soare_error.h
*/

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
