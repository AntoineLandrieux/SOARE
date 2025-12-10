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
mem *MEMORY = NULL;

////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////
MEM MemLast(MEM memory)
{
    if (!memory)
        return NULL;
    for (; memory->next; memory = memory->next)
        ;
    return memory;
}

////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////
MEM MemPushf(MEM memory, char *name, AST body)
{
    MEM mem = MemPush(memory, name, NULL);
    if (mem)
        mem->body = body;
    return mem;
}

////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////
void MemLog(MEM memory)
{
    if (!memory)
        return;

    /**
     *
     * Example:
     *
     * [MEMORY] 000001b0480d1500 <variable> (null)
     * |------> 0000000000000000 (null)
     * [MEMORY] 000001b0480d1fa0 <variable> false
     * |------> 000001b0480e3480 0
     * [MEMORY] 000001b0480ded20 <variable> true
     * |------> 000001b0480e3240 1
     * [MEMORY] 000001b0480ded50 <variable> null
     * |------> 000001b0480e3360
     * [MEMORY] 000001b0480ded80 <callable> bool
     * |------> 0000000000000000 (null)
     *
     */

    soare_write(
        //
        __soare_stdout,
        "[MEMORY] %p <%s> %s\n|------> %p %s\n",
        (void *)memory,
        memory->body ? "callable" : "variable",
        memory->name,
        (void *)memory->value,
        memory->value
        //
    );

    MemLog(memory->next);
}

#endif /* __SOARE_DEBUG */

////////////////////////////////////////////////////////////
void MemFree(MEM memory)
{
    if (!memory)
        return;

    MemFree(memory->next);
    free(memory->value);
    free(memory);
}
