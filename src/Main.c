#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <SOARE/SOARE.h>

#include "predefined.h"

uint8_t CONSOLE = 0;

static FILE *file = NULL;
static char *buffer = NULL;

/**
 * @brief Run from file using Terminal argument
 *
 * @param argc
 * @param argv
 * @return int
 */
int RunFromFile(int argc, char *argv[])
{
    CONSOLE = 0;

    int errorlevel = EXIT_SUCCESS;

    for (int i = 1; i < argc && !errorlevel; i++)
    {
        char *filename = argv[i];
        file = fopen(filename, "r");

        if (!file)
        {
            LeaveException(FileError, filename, EmptyDocument());
            return EXIT_FAILURE;
        }

        // Read file into buffer
        fseek(file, 0, SEEK_END);
        long filesize = ftell(file);
        rewind(file);

        buffer = (char *)malloc(filesize + 1);

        if (!buffer)
        {
            LeaveException(FileError, filename, EmptyDocument());
            return EXIT_FAILURE;
        }

        size_t read_size = fread(buffer, 1, filesize, file);
        buffer[read_size] = 0;

        fclose(file);
        file = NULL;

        errorlevel = Execute(filename, buffer);

        free(buffer);
        buffer = NULL;
    }

    return errorlevel;
}

static inline void Editor()
{
    size_t buffer_size = 1;

    if (!(buffer = (char *)malloc(buffer_size)))
    {
        __SOARE_OUT_OF_MEMORY();
        return;
    }

    buffer[0] = 0;

    printf(
        //
        " EDITOR MODE\n"
        " - Enter '?run' or '?commit' to execute\n"
        " - Enter '?cancel' or '?exit' to quit editor mode\n\n"
        //
    );

    while (1)
    {
        printf(
#ifdef __SOARE_COLORED_OUTPUT
            "\033[2m"
#endif
            "... "
#ifdef __SOARE_COLORED_OUTPUT
            "\033[0m"
#endif
        );

        char user[__SOARE_MAX_INPUT__];

        if (!soare_input(user))
            break;

        if (strstr(user, "?cancel") || strstr(user, "?exit"))
            break;

        if (strstr(user, "?run") || strstr(user, "?commit"))
        {
            Execute("<input>", buffer);
            break;
        }

        size_t user_len = strlen(user);
        buffer_size += user_len + 2;

        char *new_buffer = (char *)realloc(buffer, buffer_size);

        if (!new_buffer)
        {
            __SOARE_OUT_OF_MEMORY();
            break;
        }

        buffer = new_buffer;
        strcat(buffer, user);
        strcat(buffer, "\n");
    }

    free(buffer);
    buffer = NULL;
}

/**
 * @brief Console mode
 *
 * @return int
 */
int Console()
{
    CONSOLE = 1;

    printf(
        //
        "SOARE %s [%s - Antoine LANDRIEUX (MIT License)]\n"
        "<https://github.com/AntoineLandrieux/SOARE>\n"
#ifdef __SOARE_COLORED_OUTPUT
        "\033[2m"
#endif
        "Enter '?editor' to enter editor mode or '?exit' to quit."
#ifdef __SOARE_COLORED_OUTPUT
        "\033[0m"
#endif
        "\n",
        SOARE_VERSION, __PLATFORM__
        //
    );

    while (1)
    {
        printf(
#ifdef __SOARE_COLORED_OUTPUT
            "\033[35m"
#endif
            "\n>>> "
#ifdef __SOARE_COLORED_OUTPUT
            "\033[0m"
#endif
        );

        char user[__SOARE_MAX_INPUT__];

        if (!soare_input(user) || strstr(user, "?exit"))
            break;

        if (strstr(user, "?editor"))
        {
            Editor();
            continue;
        }

        Execute("<input>", user);
    }

    return EXIT_SUCCESS;
}

/**
 * @brief Main app
 *
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, char *argv[])
{
    predefined_functions();
    if (argc > 1)
        return RunFromFile(argc, argv);
    return Console();
}

/**
 * @brief Free buffer and close file if needed
 *
 */
static void __attribute__((destructor)) kill()
{
    if (file)
        fclose(file);
    if (buffer)
        free(buffer);

    if (CONSOLE)
        fprintf(stderr, "\n\n - Bye !\n\n");
}
