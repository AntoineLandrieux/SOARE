#ifndef __SOARE_MEMORY_H__
#define __SOARE_MEMORY_H__ 0x1

/* #pragma once */

/**
 *  _____  _____  ___  ______ _____
 * /  ___||  _  |/ _ \ | ___ \  ___|
 * \ `--. | | | / /_\ \| |_/ / |__
 *  `--. \| | | |  _  ||    /|  __|
 * /\__/ /\ \_/ / | | || |\ \| |___
 * \____/  \___/\_| |_/\_| \_\____/
 *
 * Antoine LANDRIEUX (MIT License) <memory.h>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

/**
 * @brief Structure of memory
 * @author Antoine LANDRIEUX
 */
typedef struct mem
{

    // Name
    char *name;
    // Value
    char *value;
    // Body
    AST body;

    // Next
    struct mem *next;

} *MEM;

/**
 * @brief Create a new empty memory
 * @author Antoine LANDRIEUX
 *
 * @return MEM
 */
MEM Mem(void);

/**
 * @brief Give the last variable in the memory
 * @author Antoine LANDRIEUX
 *
 * @param memory
 * @return MEM
 */
MEM MemLast(MEM memory);

/**
 * @brief Add a variable to an existing memory (free value if memory is NULL or if MemPush fail)
 * @author Antoine LANDRIEUX
 *
 * @param memory
 * @param name
 * @return MEM
 */
MEM MemPush(MEM memory, char *name, char *value);

/**
 * @brief Add a function to an existing memory
 * @author Antoine LANDRIEUX
 *
 * @param memory
 * @param name
 * @param body
 * @return MEM
 */
MEM MemPushf(MEM memory, char *name, AST body);

/**
 * @brief Find a variable in the memory
 * @author Antoine LANDRIEUX
 *
 * @param memory
 * @param name
 * @return MEM
 */
MEM MemGet(MEM memory, char *name);

/**
 * @brief Update a variable (free value if memory is NULL)
 * @author Antoine LANDRIEUX
 *
 * @param memory
 * @param name
 * @return MEM
 */
MEM MemSet(MEM memory, char *value);

/**
 * @brief Display all variables
 * @author Antoine LANDRIEUX
 *
 * @param memory
 */
void MemLog(MEM memory);

/**
 * @brief Join 2 memories
 * @author Antoine LANDRIEUX
 *
 * @param to
 * @param from
 */
void MemJoin(MEM to, MEM from);

/**
 * @brief Free the allocated memory
 * @author Antoine LANDRIEUX
 *
 * @param memory
 * @return void* (always returns NULL)
 */
void *MemFree(MEM memory);

#endif /* __SOARE_MEMORY_H__ */