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

/**
 * @brief Create a new empty memory
 * @author Antoine LANDRIEUX
 *
 * @return MEM
 */
MEM Mem(void)
{
    MEM memory = (MEM)malloc(sizeof(struct mem));

    if (!memory)
        return __SOARE_OUT_OF_MEMORY();

    memory->name = NULL;
    memory->next = NULL;
    memory->value = NULL;

    return memory;
}

/**
 * @brief Give the last variable in the memory
 * @author Antoine LANDRIEUX
 *
 * @param memory
 * @return MEM
 */
MEM MemLast(MEM memory)
{
    if (!memory)
        return NULL;
    MEM curr = memory;
    for (; curr->next; curr = curr->next)
        ;
    return curr;
}

/**
 * @brief Add a variable to an existing memory
 * @author Antoine LANDRIEUX
 *
 * @param memory
 * @param name
 * @return MEM
 */
MEM MemPush(MEM memory, char *name, char *value)
{
    if (!memory)
        return NULL;

    MEM mem = MemLast(memory);
    mem->next = (MEM)malloc(sizeof(struct mem));
    mem = mem->next;

    if (!mem)
        return __SOARE_OUT_OF_MEMORY();

    mem->next = NULL;
    mem->name = name;
    mem->value = value;

    return mem;
}

/**
 * @brief Find a variable in the memory
 * @author Antoine LANDRIEUX
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
    if (!get && memory->value)
        if (!strcmp(memory->name, name))
            return memory;
    return get;
}

/**
 * @brief Update a variable
 * @author Antoine LANDRIEUX
 *
 * @param memory
 * @param name
 * @return MEM
 */
MEM MemSet(MEM memory, char *value)
{
    if (!memory)
        return NULL;
    free(memory->value);
    memory->value = value;
    return memory;
}

/**
 * @brief Display all variables
 * @author Antoine LANDRIEUX
 *
 * @param memory
 */
void MemLog(MEM memory)
{
    if (!memory)
        return;
    printf(
        "[MEMORY] [%p, %s\t%s]\n",
        (void *)memory,
        memory->name,
        memory->value);
    MemLog(memory->next);
}

/**
 * @brief Join 2 memories
 * @author Antoine LANDRIEUX
 *
 * @param to
 * @param from
 */
void MemJoin(MEM to, MEM from)
{
    if (!to || !from)
        return;
    MemLast(to)->next = from;
}

/**
 * @brief Free the allocated memory
 * @author Antoine LANDRIEUX
 *
 * @param memory
 * @return void* (always returns NULL)
 */
void *MemFree(MEM memory)
{
    if (!memory)
        return NULL;

    MemFree(memory->next);
    free(memory->value);
    free(memory);
    return NULL;
}
