#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 *  _____  _____  ___  ______ _____
 * /  ___||  _  |/ _ \ | ___ \  ___|
 * \ `--. | | | / /_\ \| |_/ / |__
 *  `--. \| | | |  _  ||    /|  __|
 * /\__/ /\ \_/ / | | || |\ \| |___
 * \____/  \___/\_| |_/\_| \_\____/
 *
 * Antoine LANDRIEUX (MIT License) <Memory.c>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

#include <SOARE/SOARE.h>

// Memory used by the interpreter
MEM MEMORY = NULL;

/**
 * @brief Create a new empty memory
 *
 * @return MEM
 */
MEM Mem(void)
{
    MEM memory = (mem *)malloc(sizeof(struct mem));

    if (!memory)
        return __SOARE_OUT_OF_MEMORY();

    memory->name = NULL;
    memory->next = NULL;
    memory->body = NULL;
    memory->value = NULL;

    return memory;
}

/**
 * @brief Give the last variable in the memory
 *
 * @param memory
 * @return MEM
 */
MEM MemLast(MEM memory)
{
    if (!memory)
        return NULL;
    for (; memory->next; memory = memory->next)
        ;
    return memory;
}

/**
 * @brief Add a variable to an existing memory (free value if memory is NULL or if MemPush fail)
 *
 * @param memory
 * @param name
 * @return MEM
 */
MEM MemPush(mem *memory, char *name, char *value)
{
    if (!memory)
    {
        free(value);
        return NULL;
    }

    MEM mem = MemLast(memory);
    mem->next = (MEM)malloc(sizeof(struct mem));
    mem = mem->next;

    if (!mem)
    {
        free(value);
        return __SOARE_OUT_OF_MEMORY();
    }

    mem->next = NULL;
    mem->body = NULL;
    mem->name = name;
    mem->value = value;

    return mem;
}

/**
 * @brief Add a function to an existing memory
 *
 * @param memory
 * @param name
 * @param body
 * @return MEM
 */
MEM MemPushf(MEM memory, char *name, AST body)
{
    MEM mem = MemPush(memory, name, NULL);
    if (mem)
        mem->body = body;
    return mem;
}

/**
 * @brief Find a variable in the memory
 *
 * @param memory
 * @param name
 * @return MEM
 */
MEM MemGet(MEM memory, char *name)
{
    if (!memory)
        return NULL;
    MEM get = MemGet(memory->next, name);
    if (!get && memory->name)
        if (!strcmp(memory->name, name))
            return memory;
    return get;
}

/**
 * @brief Update a variable (free value if memory is NULL)
 *
 * @param memory
 * @param name
 * @return MEM
 */
MEM MemSet(MEM memory, char *value)
{
    if (!memory)
    {
        free(value);
        return NULL;
    }

    free(memory->value);
    memory->value = value;
    return memory;
}

#ifdef __SOARE_DEBUG

/**
 * @brief Display all variables
 *
 * @param memory
 */
void MemLog(MEM memory)
{
    if (!memory)
        return;

    /**
     *
     * Example:
     *
     * [MEMORY] [message,   "Hello World!"]
     * [MEMORY] [number,    "42"]
     *
     */

    soare_write(
        //
        __soare_stdout,
        "[MEMORY] [%s,\t\"%s\"]\n",
        memory->name,
        memory->value
        //
    );
    MemLog(memory->next);
}

#endif /* __SOARE_DEBUG */

/**
 * @brief Free the allocated memory
 *
 * @param memory
 */
void MemFree(MEM memory)
{
    if (!memory)
        return;

    MemFree(memory->next);
    free(memory->value);
    free(memory);
}
