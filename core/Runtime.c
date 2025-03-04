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
#ifdef _WIN32
#include <conio.h>
#else
// Why there are no fucking getch predefined ???
#include <termios.h>
static struct termios old, current;

char getch(void)
{
    tcgetattr(0, &old);
    current = old;
    current.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &current);
    char ch = getchar();
    tcsetattr(0, TCSANOW, &old);
    return ch;
}
#endif

MEM MEMORY = NULL;
static MEM FUNCTION = NULL;

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

    char *content = (char *)malloc(size);

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
    fclose(file);

    return ast;
}

static void InterpreterVar()
{
    MEMORY = Mem();

    MemPush(MEMORY, "__SOARE__", strdup("SOARE Antoine LANDRIEUX (MIT LICENSE)"));
    MemPush(MEMORY, "__BUILD__", strdup(__DATE__));

    MemPush(MEMORY, "__WRITE_END__", strdup("\n"));

    MemPush(MEMORY, "BC", strdup("\b"));
    MemPush(MEMORY, "CR", strdup("\r"));
    MemPush(MEMORY, "LN", strdup("\n"));
    MemPush(MEMORY, "TAB", strdup("\t"));
    MemPush(MEMORY, "CLS", strdup("\033c\033[3J"));
}

/**
 * @brief Execute a function
 * @author Antoine LANDRIEUX
 *
 * @deprecated
 *
 * @param tree
 * @return char*
 */
char *RunFunction(AST tree)
{
    AST func = BranchFind(tree->parent->child, tree->value, NODE_FUNCTION);

    if (!func)
        return LeaveException(UndefinedReference, tree->value, tree->file);

    AST ptr = func->child;
    AST src = tree->child;
    FUNCTION = Mem();

    while (ptr)
    {
        if (ptr->type == NODE_BODY)
            return Runtime(ptr);

        if (!src)
        {
            FUNCTION = MemFree(FUNCTION);
            return LeaveException(UndefinedReference, ptr->value, tree->file);
        }

        MemPush(FUNCTION, ptr->value, Eval(src));
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

    if (!MEMORY)
        InterpreterVar();

    MEM statement = MemLast(MEMORY);
    MemJoin(statement, FUNCTION);
    FUNCTION = NULL;

    for (AST curr = root->child; curr && !ErrorLevel(); curr = curr->sibling)
    {
        char *returned = NULL;
        i64 num = 0;
        AST tmp = NULL;
        MEM get = NULL;

        switch (curr->type)
        {
        case NODE_IMPORT:

            if ((tmp = loadimport(curr->value)))
                BranchJuxtapose(curr, tmp->child);
            break;

        case NODE_INPUT:

            if ((get = MemGet(MEMORY, curr->value)))
            {
                if (!(returned = malloc(2)))
                {
                    __SOARE_OUT_OF_MEMORY();
                    break;
                }
                returned[0] = (char)getch();
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

            get = MemGet(MEMORY, curr->value);

            if (!get)
            {
                LeaveException(UndefinedReference, curr->value, curr->file);
                break;
            }

            num = GetArrayIndex(curr->child, get->value);
            returned = Eval(num < 0 ? curr->child : curr->child->sibling);

            if (!returned)
                break;

            if (num >= 0)
            {
                get->value[num] = returned[0];
                free(returned);
                break;
            }

            MemSet(get, returned);
            break;

        case NODE_ENUMERATE:

            for (tmp = curr->child; tmp; tmp = tmp->sibling)
            {
                if (!(returned = malloc(((i64)(num / 10)) + 2)))
                {
                    __SOARE_OUT_OF_MEMORY();
                    break;
                }
                snprintf(returned, (((i64)(num / 10))) + 2, "%lld", num);
                MemPush(statement, tmp->value, returned);
                num += 1;
            }
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

                if (!(tmp = tmp->sibling->sibling))
                    break;

                free(returned);
                returned = Eval(tmp);
            }

            free(returned);
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

            num = (i64)AsIgnoredException();
            IgnoreException(0x1);
            returned = Runtime(curr->child);
            IgnoreException((u8)num);

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
                printf("%s%s", returned, MemGet(MEMORY, "__WRITE_END__")->value);
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
int Execute(char *file, char *rawcode)
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
