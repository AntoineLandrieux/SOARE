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
    {
        // Predefined functions
        soare_function soare_fn = soare_getfunction(tree->value);

        if (soare_fn.name)
            return soare_fn.exec(tree->child);

        // Function is not defined
        return LeaveException(UndefinedReference, tree->value, tree->file);
    }

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

static unsigned char broken = 0;

/**
 * @brief Interprets an AST node tree
 *
 * @param tree
 * @return char*
 */
static char *Runtime(AST tree)
{
    if (!tree)
        return NULL;

    MEM statement = MemLast(MEMORY);
    statement->next = FUNCTION;
    FUNCTION = NULL;

    broken = 0;

    for (AST curr = tree->child; curr && !ErrorLevel(); curr = curr->sibling)
    {
        switch (curr->type)
        {
        case NODE_FUNCTION:
            // Store function definition in current scope
            MemPushf(statement, curr->value, curr);
            break;

        case NODE_CALL:
            // Execute function call and free result
            free(RunFunction(curr));
            break;

        case NODE_BREAK:
            // Break out of loop
            broken = 1;
            return ExitStatement(statement, NULL);

        case NODE_RETURN:
            // Return from function
            return ExitStatement(statement, Eval(curr->child));

        case NODE_RAISE:
            // Raise an exception
            return ExitStatementError(statement, RaiseException, curr->value, curr->file);

        case NODE_STRERROR:
            // Store error
            MemPush(statement, curr->value, strdup(GetError()));
            break;

        case NODE_IMPORT:
        {
            // Import external file and merge its AST
            AST tmp = loadimport(curr->value);
            if (tmp)
                BranchJuxtapose(curr, tmp->child);
        }
        break;

        case NODE_MEMNEW:
            // Create new variable in current scope
            MemPush(statement, curr->value, Eval(curr->child));
            break;

        case NODE_MEMSET:
        {
            // Set variable value in current scope
            MEM get = MemGet(MEMORY, curr->value);

            if (!get)
                return ExitStatementError(statement, UndefinedReference, curr->value, curr->file);

            if (get->body)
                return ExitStatementError(statement, VariableDefinedAsFunction, curr->value, curr->file);

            MemSet(get, Eval(curr->child));
        }
        break;

        case NODE_CUSTOM_KEYWORD:
        {
            // Execute custom keyword handler
            soare_keyword keyword = soare_getkeyword(curr->value);
            if (keyword.name)
                keyword.exec();
        }
        break;

        case NODE_CONDITION:
        {
            // Evaluate condition chain (if/or/else)
            AST tmp = curr->child;
            char *condition = Eval(tmp);

            while (condition)
            {
                if (strcmp(condition, "0"))
                {
                    free(condition);

                    char *value = Runtime(tmp->sibling);

                    if (value || broken)
                        return ExitStatement(statement, value);
                    break;
                }

                free(condition);

                if (!tmp->sibling)
                    break;

                tmp = tmp->sibling->sibling;
                condition = Eval(tmp);
            }
        }
        break;

        case NODE_REPETITION:
        {
            // Loop while condition is true (!= "0")
            char *condition = Eval(curr->child);

            while (condition && strcmp(condition, "0") && !ErrorLevel() && !broken)
            {
                free(condition);

                char *value = Runtime(curr->child->sibling);

                if (value)
                    return ExitStatement(statement, value);

                condition = Eval(curr->child);
            }

            free(condition);
        }
        break;

        case NODE_TRY:
        {
            // try/iferror block
            unsigned char previous = AsIgnoredException();
            IgnoreException(1);
            char *value = Runtime(curr->child);
            IgnoreException(previous);

            if (ErrorLevel() && !broken)
            {
                free(value);
                ClearException();
                value = Runtime(curr->child->sibling);
            }

            if (value || broken)
                return ExitStatement(statement, value);
        }
        break;

        default:
            break;
        }
    }

    // Free scope and return
    return ExitStatement(statement, NULL);
}

/**
 * @brief Initialize SOARE interpreter
 *
 */
void soare_init(void)
{
    if (!MEMORY)
        MEMORY = Mem();
}

/**
 * @brief Kill SOARE interpreter
 *
 */
void soare_kill(void)
{
    MemFree(MEMORY);
    MEMORY = NULL;
}

/**
 * @brief Execute SOARE code
 *
 * @param file
 * @param rawcode
 * @return char *
 */
char *Execute(char *__restrict__ file, char *__restrict__ rawcode)
{
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
    char *value = Runtime(ast);

    // Free AST
    TreeFree(ast);

    return value;
}

#ifdef __GNUC__

static void __attribute__((destructor)) kill(void)
{
    soare_kill();
}

#endif /* __GNUC__ */
