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
 * Antoine LANDRIEUX (MIT License) <runtime.c>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

#include <SOARE/SOARE.h>

/* Stored tree */
ast_t ROOT = NULL;

/* Break */
static boolean_t scope_broken = bFalse;
/* Return */
static boolean_t scope_returned = bFalse;

////////////////////////////////////////////////////////////
static inline char *exit_scope(char *returns)
{
    soare_clear_scope();
    return returns;
}

////////////////////////////////////////////////////////////
static inline void *exit_scope_error(soare_exceptions_t error, char *string, document_t file)
{
    soare_leave_exception(error, string, file);
    soare_clear_scope();
    return NULL;
}

////////////////////////////////////////////////////////////
static inline boolean_t is_true_str(const char *str)
{
    return str && *str && strcmp(str, "0");
}

////////////////////////////////////////////////////////////
static char *runtime(ast_t tree);

////////////////////////////////////////////////////////////
static void loadimport(char *filename)
{
    FILE *file = fopen(filename, "rb");

    if (!file)
    {
        soare_leave_exception(FileError, filename, soare_empty_document());
        return;
    }

    if (fseek(file, 0, SEEK_END) != 0)
    {
        fclose(file);
        soare_leave_exception(FileError, filename, soare_empty_document());
        return;
    }

    long size = ftell(file);

    if (size <= 0)
    {
        fclose(file);
        soare_leave_exception(FileError, filename, soare_empty_document());
        return;
    }

    rewind(file);

    char *content = (char *)malloc((size_t)size + 1);

    if (!content)
    {
        fclose(file);
        SOARE_OUT_OF_MEMORY();
        return;
    }

    size_t read = fread(content, 1, (size_t)size, file);
    fclose(file);

    if (read != (size_t)size)
    {
        free(content);
        soare_leave_exception(FileError, filename, soare_empty_document());
        return;
    }

    content[read] = 0;

    tokens_t *tokens = soare_tokenizer(filename, content);
    free(content);
    ast_t ast = soare_parser(tokens);
    soare_tokens_free(tokens);
    soare_tree_juxtapose(ROOT, ast);

    soare_down_scope();
    free(runtime(ast));
    soare_up_scope();
}

////////////////////////////////////////////////////////////
char *soare_run_function(ast_t tree)
{
    soare_variables_t *get = soare_get_variable(tree->value);

    if (!get)
    {
        soare_functions_t *function = soare_get_function(tree->value);

        if (function)
        {
            return function->exec(tree->child);
        }

        soare_leave_exception(UndefinedReference, tree->value, tree->file);
        return NULL;
    }

    if (!get->body)
    {
        soare_leave_exception(ObjectIsNotCallable, tree->value, tree->file);
        return NULL;
    }

    ast_t def = get->body->child;
    ast_t arg = tree->child;

    soare_up_scope();

    while (def)
    {
        if (def->type == NODE_BODY)
        {
            soare_down_scope();
            char *returned = runtime(def);
            scope_returned = bFalse;
            return returned;
        }

        if (!arg)
        {
            soare_clear_scope();
            soare_leave_exception(MissingArgument, def->value, tree->file);
            return NULL;
        }

        char *param = soare_math(arg);
        soare_add_variable(def->value, param, bTrue);
        free(param);

        // Next argument
        arg = arg->sibling;
        def = def->sibling;
    }

    soare_clear_scope();
    return NULL;
}

////////////////////////////////////////////////////////////
static char *runtime(ast_t tree)
{
    if (!tree)
    {
        return NULL;
    }

    soare_up_scope();

    scope_broken = bFalse;
    scope_returned = bFalse;

    ast_t current = tree->child;

    while (current && !soare_errorlevel())
    {
        switch (current->type)
        {
        case NODE_RAISE:
        {
            return exit_scope_error(RaiseException, current->value, current->file);
        }

        case NODE_BREAK:
        {
            scope_broken = bTrue;
            return exit_scope(NULL);
        }

        case NODE_RETURN:
        {
            scope_returned = bTrue;
            return exit_scope(soare_math(current->child));
        }

        case NODE_IMPORT:
        {
            loadimport(current->value);
            break;
        }

        case NODE_STRERROR:
        {
            soare_add_variable(current->value, soare_get_exception(), bFalse);
            break;
        }

        case NODE_MEMNEW:
        {
            char *content = soare_math(current->child);
            soare_add_variable(current->value, content, bTrue);
            free(content);
            break;
        }

        case NODE_FUNCTION:
        {
            soare_variables_t *fn = soare_add_variable(current->value, NULL, bFalse);

            if (fn)
            {
                fn->body = current;
            }

            break;
        }

        case NODE_MEMSET:
        {
            soare_variables_t *get = soare_get_variable(current->value);

            if (!get)
            {
                return exit_scope_error(UndefinedReference, current->value, current->file);
            }

            if (get->body)
            {
                return exit_scope_error(VariableDefinedAsFunction, current->value, current->file);
            }

            if (!get->mutable)
            {
                return exit_scope_error(AssignConstantVariable, current->value, current->file);
            }

            free(get->value);
            get->value = soare_math(current->child);
            break;
        }

        case NODE_CUSTOM_KEYWORD:
        {
            soare_keywords_t *keyword = soare_get_keyword(current->value);

            if (keyword)
            {
                keyword->exec();
            }

            break;
        }

        case NODE_CONDITION:
        {
            ast_t tmp = current->child;
            char *condition = soare_math(tmp);

            while (condition)
            {
                if (is_true_str(condition))
                {
                    free(condition);
                    char *value = runtime(tmp->sibling);

                    if (value || scope_broken || scope_returned)
                    {
                        return exit_scope(value);
                    }

                    break;
                }

                free(condition);

                if (!tmp->sibling)
                {
                    break;
                }

                tmp = tmp->sibling->sibling;
                condition = soare_math(tmp);
            }
            break;
        }

        case NODE_REPETITION:
        {
            char *condition = soare_math(current->child);

            while (condition && is_true_str(condition) && !soare_errorlevel() && !scope_broken && !scope_returned)
            {
                free(condition);

                char *value = runtime(current->child->sibling);

                if (value)
                {
                    return exit_scope(value);
                }

                condition = soare_math(current->child);
            }

            free(condition);
            break;
        }

        case NODE_TRY:
        {
            boolean_t previous = soare_as_ignored_exception();
            soare_ignore_exception(bTrue);
            char *value = runtime(current->child);
            soare_ignore_exception(previous);

            if (soare_errorlevel() && !scope_broken && !scope_returned)
            {
                free(value);
                soare_clear_exception();
                value = runtime(current->child->sibling);
            }

            if (value || scope_broken || scope_returned)
            {
                return exit_scope(value);
            }

            break;
        }

        default:

            free(soare_math(current));
            break;
        }

        current = current->sibling;
    }

    return exit_scope(NULL);
}

////////////////////////////////////////////////////////////
void soare_kill(void)
{
    soare_tree_free(ROOT);

    soare_clear_keywords();
    soare_clear_functions();
    soare_clear_variables();
    soare_clear_exception();

    ROOT = NULL;
}

////////////////////////////////////////////////////////////
char *soare_execute(char *__restrict__ filename, char *__restrict__ rawcode)
{
    // Clear interpreter exception
    soare_clear_exception();

    // Interpretation step 1: Tokenizer
    tokens_t *tokens = soare_tokenizer(filename, rawcode);
    // Interpretation step 2: Parser
    ast_t ast = soare_parser(tokens);
    // Free tokens
    soare_tokens_free(tokens);
    // Save ast
    ROOT = soare_tree_juxtapose(ROOT, ast);

    // Interpretation step 3: runtime
    return runtime(ast);
}
