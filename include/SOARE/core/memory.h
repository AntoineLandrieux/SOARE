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

} mem, *MEM;

// Memory used by the interpreter
extern mem *MEMORY;

/**
 * @brief Create a new empty memory
 *
 * @return MEM
 */
MEM Mem(void);

/**
 * @brief Give the last variable in the memory
 *
 * @param memory
 * @return MEM
 */
MEM MemLast(MEM memory);

/**
 * @brief Add a variable to an existing memory (free value if memory is NULL or if MemPush fail)
 *
 * @param memory
 * @param name
 * @return MEM
 */
MEM MemPush(mem *memory, char *name, char *value);

/**
 * @brief Add a function to an existing memory
 *
 * @param memory
 * @param name
 * @param body
 * @return MEM
 */
MEM MemPushf(MEM memory, char *name, AST body);

/**
 * @brief Find a variable in the memory
 *
 * @param memory
 * @param name
 * @return MEM
 */
MEM MemGet(MEM memory, char *name);

/**
 * @brief Update a variable (free value if memory is NULL)
 *
 * @param memory
 * @param name
 * @return MEM
 */
MEM MemSet(MEM memory, char *value);

#ifdef __SOARE_DEBUG

/**
 * @brief Display all variables
 *
 * @param memory
 */
void MemLog(MEM memory);

#endif

/**
 * @brief Free the allocated memory
 *
 * @param memory
 */
void MemFree(MEM memory);

#endif /* __SOARE_MEMORY_H__ */