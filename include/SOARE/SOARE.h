#ifndef __SOARE__
#define __SOARE__ 0x1

/* #pragma once */

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

/**
 * SOARE version
 *
 * R: release
 * A: alpha
 * B: beta
 * S: snapshot
 * T: testing
 *
 */
#define SOARE_VERSION "Rv1.3.1"

/* SOARE max input */
#define __SOARE_MAX_INPUT__ 255

/* Standard input */
#define __soare_stdin stdin
/* Standard output */
#define __soare_stdout stdout
/* Standard output (error) */
#define __soare_stderr stderr

/* Output */
#define soare_write fprintf
/* Input */
#define soare_input(__buffer) fgets(__buffer, __SOARE_MAX_INPUT__, __soare_stdin)

#ifdef __cplusplus
/**
 * SOARE namespace
 */
namespace SOARE
{
    extern "C"
    {
#endif /* __cplusplus */

        /**
         * @brief Boolean
         *
         */
        typedef enum bBoolean
        {

            bFalse,
            bTrue

        } bBool;

        /**
         * @brief Structure of a document
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

#include "utils/keywords.h"
#include "utils/platform.h"

#include "core/error.h"
#include "core/tokenizer.h"
#include "core/parser.h"
#include "core/memory.h"
#include "core/math.h"
#include "core/runtime.h"

        typedef AST soare_arguments_list;

#include "core/custom.h"

#ifdef __cplusplus
    }
}
#endif /* __cplusplus */

#endif /* __SOARE__ */