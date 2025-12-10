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

static AST ROOT = NULL;
static MEM FUNCTION = NULL;

////////////////////////////////////////////////////////////
static char *ExitStatement(bBool is_root, MEM statement, char *returns)
{
    if (is_root)
        return returns;

    MemFree(statement->next);
    statement->next = NULL;
    return returns;
}

////////////////////////////////////////////////////////////
static void *ExitStatementError(MEM statement, SoareExceptions error, char *string, Document file)
{
    return ExitStatement(bFalse, statement, LeaveException(error, string, file));
}

static char *Runtime(bBool is_root, AST tree);

////////////////////////////////////////////////////////////
static void loadimport(char *filename)
{
    // Read from file
    FILE *file = fopen(filename, "rb");

    if (!file)
    {
        LeaveException(FileError, filename, EmptyDocument());
        return;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *content = (char *)malloc(size + 1);

    if (!content)
    {
        fclose(file);
        __SOARE_OUT_OF_MEMORY();
        return;
    }

    fread(content, sizeof(char), size, file);
    fclose(file);

    content[size] = 0;

    // Parse file and execute
    Tokens *tokens = Tokenizer(filename, content);
    free(content);
    AST ast = Parse(tokens);
    TokensFree(tokens);

    BranchJuxtapose(ROOT, ast);

    free(Runtime(bTrue, ast));
}

// Break
static bBool brk = bFalse;
// Return
static bBool ret = bFalse;

////////////////////////////////////////////////////////////
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

    // Memory for store argument
    MEM memf = Mem();

    while (ptr)
    {
        // Execute statement (no more argument required)
        if (ptr->type == NODE_BODY)
        {
            FUNCTION = memf;
            char *returned = Runtime(bFalse, ptr);
            ret = bFalse;
            return returned;
        }

        // Not enough argument
        if (!src)
        {
            MemFree(memf);
            return LeaveException(MissingArgument, ptr->value, tree->file);
        }

        MemPush(memf, ptr->value, Math(src));

        // Next argument
        src = src->sibling;
        ptr = ptr->sibling;
    }

    return NULL;
}

////////////////////////////////////////////////////////////
static char *Runtime(bBool is_root, AST tree)
{
    if (!tree)
        return NULL;

    MEM statement = MemLast(MEMORY);
    statement->next = FUNCTION;
    FUNCTION = NULL;

    brk = bFalse;
    ret = bFalse;

    for (AST curr = tree->child; curr && !soare_errorlevel() && !ret; curr = curr->sibling)
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
            brk = bTrue;
            return ExitStatement(is_root, statement, NULL);

        case NODE_RETURN:
            // Return from function
            ret = bTrue;
            return ExitStatement(is_root, statement, Math(curr->child));

        case NODE_RAISE:
            // Raise an exception
            return ExitStatementError(statement, RaiseException, curr->value, curr->file);

        case NODE_STRERROR:
            // Store error
            MemPush(statement, curr->value, strdup(soare_get_exception()));
            break;

        case NODE_IMPORT:
            // Import external file and merge its AST
            loadimport(curr->value);
            break;

        case NODE_MEMNEW:
            // Create new variable in current scope
            MemPush(statement, curr->value, Math(curr->child));
            break;

        case NODE_MEMSET:
        {
            // Set variable value in current scope
            MEM get = MemGet(MEMORY, curr->value);

            if (!get)
                return ExitStatementError(statement, UndefinedReference, curr->value, curr->file);

            if (get->body)
                return ExitStatementError(statement, VariableDefinedAsFunction, curr->value, curr->file);

            MemSet(get, Math(curr->child));
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
            char *condition = Math(tmp);

            while (condition)
            {
                if (strcmp(condition, "0"))
                {
                    free(condition);

                    char *value = Runtime(bFalse, tmp->sibling);

                    if (value || (brk || ret))
                        return ExitStatement(is_root, statement, value);
                    break;
                }

                free(condition);

                if (!tmp->sibling)
                    break;

                tmp = tmp->sibling->sibling;
                condition = Math(tmp);
            }
        }
        break;

        case NODE_REPETITION:
        {
            // Loop while condition is true (!= "0")
            char *condition = Math(curr->child);

            while (condition && strcmp(condition, "0") && !soare_errorlevel() && !(brk || ret))
            {
                free(condition);

                char *value = Runtime(bFalse, curr->child->sibling);

                if (value)
                    return ExitStatement(is_root, statement, value);

                condition = Math(curr->child);
            }

            free(condition);
        }
        break;

        case NODE_TRY:
        {
            // try/iferror block
            bBool previous = soare_as_ignored_exception();
            soare_ignore_exception(bTrue);
            char *value = Runtime(bFalse, curr->child);
            soare_ignore_exception(previous);

            if (soare_errorlevel() && !(brk || ret))
            {
                free(value);
                soare_clear_exception();
                value = Runtime(bFalse, curr->child->sibling);
            }

            if (value || (brk || ret))
                return ExitStatement(is_root, statement, value);
        }
        break;

        default:
            break;
        }
    }

    // Free scope and return
    return ExitStatement(is_root, statement, NULL);
}

////////////////////////////////////////////////////////////
void soare_init(void)
{
    if (!MEMORY)
        MEMORY = Mem();
}

////////////////////////////////////////////////////////////
void soare_kill(void)
{
    MemFree(MEMORY);
    TreeFree(ROOT);
    MEMORY = NULL;
    ROOT = NULL;
}

////////////////////////////////////////////////////////////
char *Execute(char *__restrict__ file, char *__restrict__ rawcode)
{
    // Clear interpreter exception
    soare_clear_exception();

    // Interpretation step 1: Tokenizer
    Tokens *tokens = Tokenizer(file, rawcode);
    // Interpretation step 2: Parser
    AST ast = Parse(tokens);

    // Free tokens
    TokensFree(tokens);

    // Save AST
    ROOT = BranchJuxtapose(ROOT, ast);

    // Interpretation step 3: Runtime
    return Runtime(bTrue, ast);
}
