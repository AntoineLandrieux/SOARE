#ifndef __SOARE_MODULE_H__
#define __SOARE_MODULE_H__

/* #pragma once */

/**
 *  _____  _____  ___  ______ _____
 * /  ___||  _  |/ _ \ | ___ \  ___|
 * \ `--. | | | / /_\ \| |_/ / |__
 *  `--. \| | | |  _  ||    /|  __|
 * /\__/ /\ \_/ / | | || |\ \| |___
 * \____/  \___/\_| |_/\_| \_\____/
 *
 * Antoine LANDRIEUX (MIT License) <module.h>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

#ifdef __cplusplus
/**
 * SOARE module namespace
 */
namespace SOARE_MODULE
{
    extern "C"
    {
#endif /* __cplusplus */

        /**
         * @brief Registers predefined functions/keywords with the SOARE runtime
         *
         * Adds system, input, write, ... functions
         * Adds true, false, null, ... variables
         */
        void load_module(void);

#ifdef __cplusplus
    }
}
#endif /* __cplusplus */

#endif /* __SOARE_MODULE_H__ */