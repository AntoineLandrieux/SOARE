#ifndef __SOARE__
#define __SOARE__ 0x1

/*
 _____  _____  ___  ______ _____
/  ___||  _  |/ _ \ | ___ \  ___|
\ `--. | | | / /_\ \| |_/ / |__
 `--. \| | | |  _  ||    /|  __|
/\__/ /\ \_/ / | | || |\ \| |___
\____/  \___/\_| |_/\_| \_\____/

 * Antoine LANDRIEUX
 * SOARE <https://github.com/AntoineLandrieux/SOARE>
 *
 * GNU General Public License v3.0
 * https://www.gnu.org/licenses/

*/

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define EQU(x, y, z) ((x) == (y) ? (z) : (y))
#define NEQ(x, y, z) ((x) != (y) ? (z) : (y))

#define SOARE_VERSION_MAJOR 0x01
#define SOARE_VERSION_MINOR 0x00
#define SOARE_VERSION_PATCH 0x00

#define SOARE_MAX_INPUT 0xFF

#define SOARE_NULL "0"
#define SOARE_NaN "NaN"
#define SOARE_UNDEFINED "undefined"

#ifdef __cplusplus
namespace SOARE
{
    extern "C"
    {
#endif

#include <SOARE/SAE/sae_error.h>
#include <SOARE/SAE/sae_utils.h>
#include <SOARE/SAE/sae_token.h>
#include <SOARE/SAE/sae_nodes.h>
#include <SOARE/SAE/sae_def.h>
#include <SOARE/SAE/sae_run.h>

#ifdef __cplusplus
    }
}
#endif

#endif /* __SOARE__ */