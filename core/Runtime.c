
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

static MEM FUNCTION = NULL;

/**
 * @brief Exit current statement #Runtime(AST)
 *
 * @param returns
 * @return char*
 */
static char *ExitStatement(MEM statement, char *returns)
{
    MemFree(statement->next);
    statement->next = NULL;
    return returns;
}

/**
 * @brief Exit current statement an error #Runtime(AST)
 *
 * @param error
 * @param string
 * @param file
 * @return void*
 */
static void *ExitStatementError(MEM statement, SoareExceptions error, char *string, Document file)
{
    return ExitStatement(statement, LeaveException(error, string, file));
}

/**
 * @brief Executes code from a tree
 *
 * @param tree
 * @return char*
 */
static char *Runtime(AST tree);

/**
 * @brief User input
 *
 * @return char*
 */
static char *input()
{
    char string[__SOARE_MAX_INPUT__] = {0};
    soare_input(string);

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

    free(content);
    TokensFree(tokens);
    fclose(file);

    return ast;
}

/**
 * @brief Set Interpreter variables
 *
 * @param file
 */
static void InterpreterVar(char *file)
{
    MEMORY = Mem();

    /* SOARE version */
    MemPush(MEMORY, "__SOARE__", strdup(SOARE_VERSION));
    /* Current file */
    MemPush(MEMORY, "__FILE__", strdup(file));
    /* Path to SOARE executable */
    MemPush(MEMORY, "__ENVIRONMENT__", strdup(GetEnvironment()));
    /* Errors */
    MemPush(MEMORY, "__ERROR__", strdup("NoError"));
    /* Build date */
    MemPush(MEMORY, "__BUILD__", strdup(__DATE__));
    /* Current OS */
    MemPush(MEMORY, "__PLATFORM__", strdup(__PLATFORM__));
}

/**
 * @brief Execute a function
 *
 * @param tree
 * @return char*
 */
char *RunFunction(AST tree)
{
    // Get the memory
    MEM get = MemGet(MEMORY, tree->value);

    // Memory not found
    if (!get)
        return LeaveException(UndefinedReference, tree->value, tree->file);

    // Memory is not a function
    if (!get->body)
        return LeaveException(ObjectIsNotCallable, tree->value, tree->file);

    // Arguments
    AST ptr = get->body->child;
    AST src = tree->child;
    MEM memf = Mem();

    AST func = NULL;

    while (ptr)
    {
        // Execute statement (no more argument required)
        if (ptr->type == NODE_BODY)
        {
            FUNCTION = memf;
            return Runtime(ptr);
        }

        // Not enough argument
        if (!src)
        {
            MemFree(memf);
            memf = NULL;
            return LeaveException(UndefinedReference, ptr->value, tree->file);
        }

        get = NULL;

        // If it is a reference to a memory
        if (src->type == NODE_MEMGET)
            // If memory exists
            if ((get = MemGet(MEMORY, src->value)))
                // If memory is a function
                if ((func = get->body))
                    // Add this function in argument
                    get = MemPushf(memf, ptr->value, func);

        // If argument is not a function
        if (!get || !func)
            MemPush(memf, ptr->value, Eval(src));

        // Next argument
        src = src->sibling;
        ptr = ptr->sibling;
    }

    return NULL;
}

static char broken = 0;

/**
 * @brief Executes code from a tree
 *
 * @param tree
 * @return char*
 */
static char *Runtime(AST tree)
{
    if (!tree)
        return NULL;

    AST root = tree;
    AST tmp = NULL;
    MEM get = NULL;

    char *returned = NULL;
    unsigned char error = 0;

    broken = 0;

    MEM statement = MemLast(MEMORY);
    MemJoin(statement, FUNCTION);
    FUNCTION = NULL;

    for (AST curr = root->child; curr && !ErrorLevel(); curr = curr->sibling)
    {
        switch (curr->type)
        {
        // Execute shell code
        case NODE_SHELL:

            returned = Eval(curr->child);
            system(returned);
            free(returned);
            break;

        // Store function into MEMORY
        case NODE_FUNCTION:

            MemPushf(statement, curr->value, curr);
            break;

        // Import SOARE code from an other file
        case NODE_IMPORT:

            if ((tmp = loadimport(curr->value)))
                BranchJuxtapose(curr, tmp->child);
            break;

        // User input
        case NODE_INPUT:

            if ((get = MemGet(MEMORY, curr->value)))
            {
                if (!(returned = input()))
                    break;

                MemSet(get, returned);
                break;
            }

            return ExitStatementError(statement, UndefinedReference, curr->value, curr->file);

        // Call a function
        case NODE_CALL:

            free(RunFunction(curr));
            break;

        // Push a new variable into MEMORY
        case NODE_MEMNEW:

            MemPush(statement, curr->value, Eval(curr->child));
            break;

        // Set a value to a variable
        case NODE_MEMSET:

            if (!(get = MemGet(MEMORY, curr->value)))
                return ExitStatementError(statement, UndefinedReference, curr->value, curr->file);

            if (get->body)
                return ExitStatementError(statement, VariableDefinedAsFunction, curr->value, curr->file);

            MemSet(get, Eval(curr->child));
            break;

        // Condition statement (if)
        case NODE_CONDITION:

            returned = Eval(curr->child);
            tmp = curr->child;

            while (returned)
            {
                if (strcmp(returned, "0"))
                {
                    free(returned);

                    if ((returned = Runtime(tmp->sibling)) || broken)
                        return ExitStatement(statement, returned);

                    break;
                }

                free(returned);

                if (!(tmp = tmp->sibling->sibling))
                    break;

                returned = Eval(tmp);
            }
            break;

        // Repetition statement (while)
        case NODE_REPETITION:

            while ((returned = Eval(curr->child)))
            {
                if (!strcmp(returned, "0") || ErrorLevel() || broken)
                    break;

                free(returned);

                if ((returned = Runtime(curr->child->sibling)))
                    return ExitStatement(statement, returned);
            }

            free(returned);
            break;

        // try/iferror statement
        case NODE_TRY:

            error = AsIgnoredException();
            IgnoreException(1);
            returned = Runtime(curr->child);
            IgnoreException(error);

            if (ErrorLevel() && !broken)
            {
                free(returned);
                ClearException();
                returned = Runtime(curr->child->sibling);
            }

            if (!returned && !broken)
                break;

            return ExitStatement(statement, returned);

        // Print
        case NODE_OUTPUT:

            if ((returned = Eval(curr->child)))
            {
                soare_write(__soare_stdout, "%s", returned);
                free(returned);
            }
            break;

        // Break loop
        case NODE_BREAK:

            broken = 1;
            return ExitStatement(statement, NULL);

        // Return
        case NODE_RETURN:

            return ExitStatement(statement, Eval(curr->child));

        // Leave new exception
        case NODE_RAISE:

            return ExitStatementError(statement, RaiseException, curr->value, curr->file);

        default:

            break;
        }
    }

    // Quit
    return ExitStatement(statement, NULL);
}

/**
 * @brief Execute the code from a string
 *
 * @param rawcode
 */
int Execute(char *__restrict__ file, char *__restrict__ rawcode)
{
    if (!MEMORY)
        // Set default vars..
        InterpreterVar(file);

    // Clear interpreter exception
    ClearException();

    // Interpretation step 1: Tokenizer
    Tokens *tokens = Tokenizer(file, rawcode);
    // Interpretation step 2: Parser
    AST ast = Parse(tokens);

#ifdef __SOARE_DEBUG
    // DEBUG: print tokens and trees
    TokensLog(tokens);
    TreeLog(ast);
#endif

    // Free tokens
    TokensFree(tokens);

    // Interpretation step 3: Runtime
    free(Runtime(ast));

    // Free AST
    TreeFree(ast);
    // Free MEMORY
    MemFree(MEMORY);

    // Set MEMORY to NULL
    MEMORY = NULL;

    // Return error level
    // 0: EXIT_SUCCESS
    // 1: EXIT_FAILURE
    return (int)ErrorLevel();
}
