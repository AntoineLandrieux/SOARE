
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
 * Antoine LANDRIEUX (MIT License) <Runtime.c>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

#include <SOARE/SOARE.h>

MEM MEMORY = NULL;
static MEM FUNCTION = NULL;

/**
 * @brief Executes code from a tree
 * @author Antoine LANDRIEUX
 *
 * @param tree
 * @return char*
 */
char *Runtime(AST tree);

char *input()
{
    char string[__SOARE_MAX_INPUT__] = {0};
    fgets(string, sizeof(string), stdin);

    size_t size = strlen(string) - 1;
    string[size] = 0;

    char *result = malloc(size);

    if (!result)
        return __SOARE_OUT_OF_MEMORY();

    strcpy(result, string);
    return result;
}

/**
 * @brief From file file
 * @author Antoine LANDRIEUX
 *
 * @param filename
 * @return AST
 */
static AST loadimport(char *filename)
{
    FILE *file = fopen(filename, "rb");

    if (!file)
        return LeaveException(FileError, filename, EmptyDocument());

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *content = (char *)malloc(size + 1);

    if (!content)
    {
        fclose(file);
        return LeaveException(InterpreterError, filename, EmptyDocument());
    }

    fread(content, sizeof(char), size, file);
    content[size] = 0;

    Tokens *tokens = Tokenizer(filename, content);
    AST ast = Parse(tokens);

#ifdef __SOARE_DEBUG
    TokensLog(tokens);
    TreeLog(ast);
#endif

    free(content);
    TokensFree(tokens);
    fclose(file);

    return ast;
}

static void InterpreterVar()
{
    MEMORY = Mem();

    MemPush(MEMORY, "__SOARE__", strdup("SOARE (MIT LICENSE)"));
    MemPush(MEMORY, "__ERROR__", strdup("NoError"));
    MemPush(MEMORY, "__BUILD__", strdup(__DATE__));
    MemPush(MEMORY, "__PLATFORM__", strdup(__PLATFORM__));
}

/**
 * @brief Execute a function
 * @author Antoine LANDRIEUX
 *
 * @param tree
 * @return char*
 */
char *RunFunction(AST tree)
{
    MEM get = MemGet(MEMORY, tree->value);
    AST func = NULL;

    if (!get)
        return LeaveException(UndefinedReference, tree->value, tree->file);

    if (!(func = get->body))
        return LeaveException(ObjectIsNotCallable, tree->value, tree->file);

    AST ptr = func->child;
    AST src = tree->child;
    MEM memf = Mem();

    func = NULL;

    while (ptr)
    {
        if (ptr->type == NODE_BODY)
        {
            FUNCTION = memf;
            return Runtime(ptr);
        }

        if (!src)
        {
            memf = MemFree(memf);
            return LeaveException(UndefinedReference, ptr->value, tree->file);
        }

        get = NULL;

        if (src->type == NODE_MEMGET)
            if ((get = MemGet(MEMORY, src->value)))
                if ((func = get->body))
                    get = MemPushf(memf, ptr->value, func);

        if (!get || !func)
            MemPush(memf, ptr->value, Eval(src));

        src = src->sibling;
        ptr = ptr->sibling;
    }

    return NULL;
}

/**
 * @brief Executes code from a tree
 * @author Antoine LANDRIEUX
 *
 * @param tree
 * @return char*
 */
char *Runtime(AST tree)
{
    if (!tree)
        return NULL;

    AST root = tree;
    AST tmp = NULL;
    MEM get = NULL;
    Tokens *tokens = NULL;
    char *returned = NULL;

    if (!MEMORY)
        InterpreterVar();

    MEM statement = MemLast(MEMORY);
    MemJoin(statement, FUNCTION);
    FUNCTION = NULL;

    for (AST curr = root->child; curr && !ErrorLevel(); curr = curr->sibling)
    {
        long long num = 0;

        switch (curr->type)
        {
        case NODE_SHELL:

            returned = Eval(curr->child);
            system(returned);
            free(returned);
            break;

        case NODE_REINTERPRET:

            returned = Eval(curr->child);
            tokens = Tokenizer("<SOARE::REINTERPRET>", returned);
            tmp = Parse(tokens);
            free(Runtime(tmp));
            TokensFree(tokens);
            TreeFree(tmp);
            free(returned);
            break;

        case NODE_FUNCTION:

            MemPushf(statement, curr->value, curr);
            break;

        case NODE_IMPORT:

            if ((tmp = loadimport(curr->value)))
                BranchJuxtapose(curr, tmp->child);
            break;

        case NODE_INPUT:

            if ((get = MemGet(MEMORY, curr->value)))
            {
                if (!(returned = input()))
                    break;
                MemSet(get, returned);
                break;
            }
            LeaveException(UndefinedReference, curr->value, curr->file);
            break;

        case NODE_CALL:

            free(RunFunction(curr));
            break;

        case NODE_MEMNEW:

            MemPush(statement, curr->value, Eval(curr->child));
            break;

        case NODE_MEMSET:

            if (!(get = MemGet(MEMORY, curr->value)))
            {
                LeaveException(UndefinedReference, curr->value, curr->file);
                break;
            }

            if (get->body)
            {
                LeaveException(VariableDefinedAsFunction, curr->value, curr->file);
                break;
            }

            returned = Eval(curr->child);

            if (!returned)
                break;

            MemSet(get, returned);
            break;

        case NODE_CONDITION:

            returned = Eval(curr->child);
            tmp = curr->child;

            while (returned)
            {
                if (strcmp(returned, "0"))
                {
                    free(returned);
                    if ((returned = Runtime(tmp->sibling)))
                    {
                        statement->next = MemFree(statement->next);
                        return returned;
                    }
                    break;
                }

                free(returned);
                if (!(tmp = tmp->sibling->sibling))
                    break;
                returned = Eval(tmp);
            }
            break;

        case NODE_REPETITION:

            while ((returned = Eval(curr->child)))
            {
                if (!strcmp(returned, "0") || ErrorLevel())
                    break;
                free(returned);
                if ((returned = Runtime(curr->child->sibling)))
                {
                    statement->next = MemFree(statement->next);
                    return returned;
                }
            }
            free(returned);
            break;

        case NODE_TRY:

            num = (long long)AsIgnoredException();
            IgnoreException(0x1);
            returned = Runtime(curr->child);
            IgnoreException((unsigned char)num);

            if (ErrorLevel())
            {
                free(returned);
                ClearException();
                returned = Runtime(curr->child->sibling);
            }

            if (!returned)
                break;

            statement->next = MemFree(statement->next);
            return returned;

        case NODE_OUTPUT:

            if ((returned = Eval(curr->child)))
                printf("%s", returned);
            break;

        case NODE_RETURN:

            returned = Eval(curr->child);
            statement->next = MemFree(statement->next);
            return returned;

        case NODE_RAISE:

            statement->next = MemFree(statement->next);
            return LeaveException(RaiseException, curr->value, curr->file);

        default:
            break;
        }
    }

    statement->next = MemFree(statement->next);
    return NULL;
}

/**
 * @brief Execute the code from a string
 * @author Antoine LANDRIEUX
 *
 * @param rawcode
 */
int Execute(char *__restrict__ file, char *__restrict__ rawcode)
{
    ClearException();
    Tokens *tokens = Tokenizer(file, rawcode);
    AST ast = Parse(tokens);

#ifdef __SOARE_DEBUG
    TokensLog(tokens);
    TreeLog(ast);
#endif

    free(Runtime(ast));

    MEMORY = MemFree(MEMORY);
    TokensFree(tokens);
    TreeFree(ast);

    return (int)ErrorLevel();
}
