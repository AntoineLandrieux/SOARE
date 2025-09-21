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

    for (int i = 1; i < argc; i++)
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

        soare_init();
        free(Execute(filename, buffer));
        soare_kill();

        free(buffer);
        buffer = NULL;
    }

    return EXIT_SUCCESS;
}

/**
 * @brief Run from user input
 *
 * @param user
 */
static void RunInput(char *user)
{
    soare_init();

    char *value = Execute("<input>", user);

    if (value)
    {
        printf(
#ifdef __SOARE_COLORED_OUTPUT
            "\033[32m"
#endif
            "\n%s"
#ifdef __SOARE_COLORED_OUTPUT
            "\033[0m"
#endif
            ,
            value
            //
        );
        free(value);
    }

    soare_kill();
}

/**
 * @brief Editor mode
 *
 */
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
#ifdef __SOARE_COLORED_OUTPUT
        "\033[2m"
#endif
        "\n EDITOR MODE\n"
        " - Enter '?run' to execute\n"
        " - Enter '?cancel' to quit editor mode\n\n"
#ifdef __SOARE_COLORED_OUTPUT
        "\033[0m"
#endif
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

        if (strstr(user, "?cancel") || strstr(user, "?exit"))
            break;

        if (strstr(user, "?run"))
        {
            RunInput(buffer);
            break;
        }
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

        if (!soare_input(user))
            break;

        RunInput(user);

        if (strstr(user, "?exit"))
            break;

        if (strstr(user, "?editor"))
            Editor();
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

#ifdef __GNUC__

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

#endif /* __GNUC__ */
