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
 * @def SOARE_VERSION
 * @brief SOARE version string.
 *
 * The string follows the project's internal versioning format
 * (prefix letter + semantic version).
 */
#define SOARE_VERSION "Rv1.3.2"

/* SOARE max input */
#define __SOARE_MAX_INPUT__ 255

/* Standard input */
#define __soare_stdin stdin
/* Standard output */
#define __soare_stdout stdout
/* Standard output (error) */
#define __soare_stderr stderr

/** @brief Print helper (wrapper for fprintf). */
#define soare_write fprintf
/** @brief Read a line from standard input into `__buffer`. */
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
         * @enum boolean
         * @brief Simple boolean type used throughout the codebase.
         */
        typedef enum boolean
        {

            bFalse, /**< False value (0) */
            bTrue   /**< True value (non-zero) */

        } boolean_t;

        /**
         * @struct document
         * @brief Source location descriptor used for diagnostics.
         */
        typedef struct document
        {

            char *filename;         /**< Filename */
            unsigned long long ln;  /**< 1-based line number */
            unsigned long long col; /**< 1-based column number */

        } document_t;

#include "utils/keywords.h"
#include "utils/platform.h"

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