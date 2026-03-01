#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <SOARE/SOARE.h>

#include "../modules/module.h"

/**
 *  _____  _____  ___  ______ _____
 * /  ___||  _  |/ _ \ | ___ \  ___|
 * \ `--. | | | / /_\ \| |_/ / |__
 *  `--. \| | | |  _  ||    /|  __|
 * /\__/ /\ \_/ / | | || |\ \| |___
 * \____/  \___/\_| |_/\_| \_\____/
 *
 * Antoine LANDRIEUX (MIT License) <Main.c>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

static char *buffer = NULL;

////////////////////////////////////////////////////////////
static char *append(const char *str1, const char *str2)
{
    char *result = (char *)malloc(strlen(str1) + strlen(str2) + 1);

    if (!result)
    {
        SOARE_OUT_OF_MEMORY();
        return NULL;
    }

    strcpy(result, str1);
    strcat(result, str2);

    return result;
}

////////////////////////////////////////////////////////////
static void interpreter_kill(int sig)
{
    // Kill SOARE
    signal(sig, SIG_DFL);
    exit(sig);
}

////////////////////////////////////////////////////////////
static void interpreter_at_exit(void)
{
    soare_kill();
    free(buffer);

#ifdef __KILL_MESSAGE
    fprintf(stderr, "\n\n%s\n\n", __KILL_MESSAGE);
#endif
}

////////////////////////////////////////////////////////////
static inline void handle_signal(void)
{
    atexit(interpreter_at_exit);

    signal(SIGINT, /*  */ interpreter_kill);
    signal(SIGTERM, /* */ interpreter_kill);
    signal(SIGABRT, /* */ interpreter_kill);
    signal(SIGSEGV, /* */ interpreter_kill);
}

////////////////////////////////////////////////////////////
static void loadfile(const char *filename)
{
    FILE *file = fopen(filename, "r");

    if (!file)
    {
        soare_leave_exception(FileError, filename, soare_empty_document());
        return;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    buffer = (char *)malloc(size + 1);

    if (!buffer)
    {
        SOARE_OUT_OF_MEMORY();
        return;
    }

    size_t read_size = fread(buffer, sizeof(char), size, file);
    buffer[read_size] = 0;
    fclose(file);
}

////////////////////////////////////////////////////////////
int Files(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        loadfile(argv[i]);
        free(soare_execute(argv[i], buffer));
        free(buffer);
        buffer = NULL;
    }

    interpreter_at_exit();
    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////
int Console()
{
    printf(
        //
        "SOARE " SOARE_VERSION " [" __PLATFORM__ " - Antoine LANDRIEUX (MIT License)]\n"
        "<https://github.com/AntoineLandrieux/SOARE>\n"
        "\033[2mPress Ctrl+C or type exit() to exit\033[0m\n"
        //
    );

#define MAX_INPUT 255
    char user[MAX_INPUT];

    while (1)
    {
        soare_clear_exception();

        do
        {
            if (!buffer)
            {
                // >>> write("hello")
                // hello
                // >>>
                buffer = strdup("");
                printf("\033[35m\n>>> \033[0m");
            }
            else
            {
                // >>> if (1)
                // ...   write("hello")
                // ... end
                // hello
                // >>>
                printf("\033[0;2m... \033[0m");
            }

            if (!soare_input(user, MAX_INPUT))
            {
                break;
            }

            if (!buffer)
            {
                SOARE_OUT_OF_MEMORY();
                return EXIT_FAILURE;
            }

            buffer = append(buffer, user);

            // Parse for check if all statement closed
            tokens_t *tokens = soare_tokenizer("<input>", buffer);
            soare_tree_free(soare_parser(tokens));
            soare_tokens_free(tokens);

        } while (!soare_is_all_statement_closed() && !soare_errorlevel());

        if (!soare_errorlevel())
        {
            char *result = soare_execute("<input>", buffer);

            if (result)
            {
                printf("\033[32m\n%s\033[0m", result);
                free(result);
            }
        }

        free(buffer);
        buffer = NULL;
    }

    interpreter_at_exit();
    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    handle_signal();
    load_module();

    if (argc > 1)
    {
        return Files(argc, argv);
    }

    return Console();
}
