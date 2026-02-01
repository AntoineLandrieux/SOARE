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

/* Number of defined scopes */
static unsigned long long scope = 0;
/* Break */
static boolean_t scope_broken = bFalse;
/* Return */
static boolean_t scope_returned = bFalse;

////////////////////////////////////////////////////////////
static inline char *exit_scope(boolean_t is_root, char *returns)
{
    if (!is_root)
        soare_clear_variables_scope(scope);
    scope -= 1;
    return returns;
}

////////////////////////////////////////////////////////////
static inline void *exit_scope_error(boolean_t is_root, soare_exceptions_t error, char *string, document_t file)
{
    return exit_scope(is_root, soare_leave_exception(error, string, file));
}

////////////////////////////////////////////////////////////
static inline boolean_t is_true_str(const char *str)
{
    return str && strcmp(str, "0");
}

////////////////////////////////////////////////////////////
static char *runtime(boolean_t is_root, ast_t tree);

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
        __SOARE_OUT_OF_MEMORY();
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

    scope -= 1;
    free(runtime(bTrue, ast));
    scope += 1;
}

////////////////////////////////////////////////////////////
char *soare_run_function(ast_t tree)
{
    soare_variables_t *get = soare_get_variable(tree->value);

    if (!get)
    {
        soare_functions_t *function = soare_get_function(tree->value);

        if (function)
            return function->exec(tree->child);

        return soare_leave_exception(UndefinedReference, tree->value, tree->file);
    }

    if (!get->body)
        return soare_leave_exception(ObjectIsNotCallable, tree->value, tree->file);

    ast_t def = get->body->child;
    ast_t arg = tree->child;

    while (def)
    {
        if (def->type == NODE_BODY)
        {
            char *returned = runtime(bFalse, def);
            scope_returned = bFalse;
            return returned;
        }

        if (!arg)
        {
            soare_clear_variables_scope(scope + 1);
            return soare_leave_exception(MissingArgument, def->value, tree->file);
        }

        soare_variables_t *param = soare_add_variable(def->value, NULL, 1);

        if (param)
        {
            param->value = soare_math(arg);
            param->scope = scope + 1;
        }

        // Next argument
        arg = arg->sibling;
        def = def->sibling;
    }

    return NULL;
}

////////////////////////////////////////////////////////////
static char *runtime(boolean_t is_root, ast_t tree)
{
    if (!tree)
        return NULL;

    scope += 1;
    scope_broken = bFalse;
    scope_returned = bFalse;

    for (ast_t curr = tree->child; curr && !soare_errorlevel() && !scope_returned; curr = curr->sibling)
    {
        switch (curr->type)
        {
        case NODE_RAISE:
            return exit_scope_error(is_root, RaiseException, curr->value, curr->file);

        case NODE_BREAK:
            scope_broken = bTrue;
            return exit_scope(is_root, NULL);

        case NODE_RETURN:
            scope_returned = bTrue;
            return exit_scope(is_root, soare_math(curr->child));

        case NODE_IMPORT:
            loadimport(curr->value);
            break;

        case NODE_MEMNEW:
        {
            soare_variables_t *var = soare_add_variable(curr->value, NULL, bTrue);

            if (var)
            {
                var->value = soare_math(curr->child);
                var->scope = scope;
            }

            break;
        }

        case NODE_STRERROR:
        {
            soare_variables_t *err = soare_add_variable(curr->value, NULL, bFalse);

            if (err)
            {
                err->value = strdup(soare_get_exception());
                err->scope = scope;
            }

            break;
        }

        case NODE_FUNCTION:
        {
            soare_variables_t *fn = soare_add_variable(curr->value, NULL, bFalse);

            if (fn)
            {
                fn->body = curr;
                fn->scope = scope;
            }

            break;
        }

        case NODE_MEMSET:
        {
            soare_variables_t *get = soare_get_variable(curr->value);

            if (!get)
                return exit_scope_error(is_root, UndefinedReference, curr->value, curr->file);

            if (get->body)
                return exit_scope_error(is_root, VariableDefinedAsFunction, curr->value, curr->file);

            if (!get->mutable)
                return exit_scope_error(is_root, AssignConstantVariable, curr->value, curr->file);

            free(get->value);
            get->value = soare_math(curr->child);
            break;
        }

        case NODE_CUSTOM_KEYWORD:
        {
            soare_keywords_t *keyword = soare_get_keyword(curr->value);

            if (keyword)
                keyword->exec();

            break;
        }

        case NODE_CONDITION:
        {
            ast_t tmp = curr->child;
            char *condition = soare_math(tmp);

            while (condition)
            {
                if (is_true_str(condition))
                {
                    free(condition);
                    char *value = runtime(bFalse, tmp->sibling);

                    if (value || scope_broken || scope_returned)
                        return exit_scope(is_root, value);

                    break;
                }

                free(condition);

                if (!tmp->sibling)
                    break;

                tmp = tmp->sibling->sibling;
                condition = soare_math(tmp);
            }
            break;
        }

        case NODE_REPETITION:
        {
            char *condition = soare_math(curr->child);

            while (condition && is_true_str(condition) && !soare_errorlevel() && !scope_broken && !scope_returned)
            {
                free(condition);

                char *value = runtime(bFalse, curr->child->sibling);

                if (value)
                    return exit_scope(is_root, value);

                condition = soare_math(curr->child);
            }

            free(condition);
            break;
        }

        case NODE_TRY:
        {
            boolean_t previous = soare_as_ignored_exception();
            soare_ignore_exception(bTrue);
            char *value = runtime(bFalse, curr->child);
            soare_ignore_exception(previous);

            if (soare_errorlevel() && !scope_broken && !scope_returned)
            {
                free(value);
                soare_clear_exception();
                value = runtime(bFalse, curr->child->sibling);
            }

            if (value || scope_broken || scope_returned)
                return exit_scope(is_root, value);

            break;
        }

        default:

            free(soare_math(curr));
            break;
        }
    }

    return exit_scope(is_root, NULL);
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
    return runtime(bTrue, ast);
}
