#ifndef __SOARE__
#define __SOARE__ 0x1

/* #prama once */

/**
 *  _____  _____  ___  ______ _____
 * /  ___||  _  |/ _ \ | ___ \  ___|
 * \ `--. | | | / /_\ \| |_/ / |__
 *  `--. \| | | |  _  ||    /|  __|
 * /\__/ /\ \_/ / | | || |\ \| |___
 * \____/  \___/\_| |_/\_| \_\____/
 *
 * Antoine LANDRIEUX (MIT License) <SOARE.h>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

#ifdef __cplusplus
/**
 * SOARE namespace
 */
namespace SOARE
{
    extern "C"
    {
#endif /* __cplusplus */

/* SOARE major */
#define SOARE_MAJOR 1
/* SOARE minor */
#define SOARE_MINOR 0
/* SOARE patch */
#define SOARE_PATCH 3

        /**
         * @brief Structure of a document
         * @author Antoine LANDRIEUX
         */
        typedef struct Document
        {

            // File
            char *file;

            // Line
            unsigned long long ln;
            // Column
            unsigned long long col;

        } Document;

/* Input file for console mode */
#define __SOARE_FILE__ "_input"

#include "utils/int.h"
#include "utils/keywords.h"

#include "core/error.h"
#include "core/tokenizer.h"
#include "core/parser.h"
#include "core/memory.h"
#include "core/math.h"
#include "core/runtime.h"

#ifdef __cplusplus
    }
}
#endif /* __cplusplus */

#endif /* __SOARE__ */