#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <SOARE/SOARE.h>

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

/**
 * @brief Parse preprocessor
 *
 * @param user
 * @param buf
 * @param buf_size
 * @return size_t
 */
static size_t ParsePreprocessor(char *user, size_t buf_size)
{
    if (strstr(user, "?run"))
    {
        Execute("input", buffer);
    }

    if (strstr(user, "?commit"))
    {
        Execute("input", buffer);
        free(buffer);
        buf_size = 0;
        buffer = NULL;
    }

    if (strstr(user, "?cancel"))
    {
        free(buffer);
        buf_size = 0;
        buffer = NULL;
    }

    if (strstr(user, "?clear"))
    {
        printf("\033c\033[3J");
    }

    if (strstr(user, "?exit"))
    {
        exit(EXIT_SUCCESS);
    }

    return buf_size;
}

/**
 * @brief Console mode
 *
 * @return int
 */
int Console()
{
    CONSOLE = 1;

    size_t buf_size = 0;

    printf(
        //
        "SOARE %s [%s - Antoine LANDRIEUX (MIT License)]\n"
        "<https://github.com/AntoineLandrieux/SOARE>\n"
#ifdef __SOARE_COLORED_OUTPUT
        "\033[2m"
#endif
        "Enter '?run' or '?commit' to run code or '?exit' to quit."
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

        if (!fgets(user, sizeof(user), stdin))
            break;

        size_t user_len = strlen(user);

        if (user_len > 0 && user[user_len - 1] == '\n')
            user[user_len - 1] = 0;

        // Append user input to buffer
        size_t new_size = buf_size + user_len + 2;
        char *new_buf = (char *)realloc(buffer, new_size);

        if (!new_buf)
        {
            __SOARE_OUT_OF_MEMORY();
            return EXIT_FAILURE;
        }

        buffer = new_buf;

        if (buf_size == 0)
            buffer[0] = 0;

        strcat(buffer, user);
        strcat(buffer, "\n");

        buf_size = ParsePreprocessor(user, strlen(buffer));
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
    SetEnvironment(argv[0]);
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
