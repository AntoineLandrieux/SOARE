#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SOARE/SOARE.h"

/**
 * @brief Run from file using Terminal argument
 *
 * @param argc
 * @param argv
 * @return int
 */
int RunFromFile(int argc, char *argv[])
{
    int errorlevel = EXIT_SUCCESS;

    for (int i = 1; i < argc; i++)
    {
        char *filename = argv[i];
        FILE *file = fopen(filename, "r");

        if (!file)
        {
            LeaveException(FileError, filename, EmptyDocument());
            return EXIT_FAILURE;
        }

        // Read file into buffer
        fseek(file, 0, SEEK_END);
        long filesize = ftell(file);
        rewind(file);

        char *buffer = (char *)malloc(filesize + 1);

        if (!buffer)
        {
            fclose(file);
            LeaveException(FileError, filename, EmptyDocument());
            return EXIT_FAILURE;
        }

        size_t read_size = fread(buffer, 1, filesize, file);
        buffer[read_size] = '\0';
        fclose(file);

        int _error = Execute(filename, buffer);
        errorlevel = _error || errorlevel;

        free(buffer);
    }

    return errorlevel;
}

/**
 * @brief Console mode
 *
 * @return int
 */
int Console()
{
    char *exe = NULL;
    size_t exe_size = 0;

    printf(
        "SOARE %s [%s - Antoine LANDRIEUX (MIT License)]\n"
        "<https://github.com/AntoineLandrieux/SOARE>\n"
#ifdef __SOARE_COLORED_OUTPUT
        "\033[2m"
#endif
        "Enter '?run' or '?commit' to run code or '?exit' to quit.\n"
#ifdef __SOARE_COLORED_OUTPUT
        "\033[0m"
#endif
        "\n",
        SOARE_VERSION, __PLATFORM__
    );

    while (1)
    {
#ifdef __SOARE_COLORED_OUTPUT
        printf("\033[35m");
#endif
        printf(">>> ");
#ifdef __SOARE_COLORED_OUTPUT
        printf("\033[0m");
#endif

        char user[4096];
        if (!fgets(user, sizeof(user), stdin))
            break;

        size_t user_len = strlen(user);
        if (user_len > 0 && user[user_len - 1] == '\n')
            user[user_len - 1] = '\0';

        // Append user input to exe buffer
        size_t new_size = exe_size + user_len + 2;
        char *new_exe = (char *)realloc(exe, new_size);

        if (!new_exe)
        {
            free(exe);
            __SOARE_OUT_OF_MEMORY();
            return EXIT_FAILURE;
        }

        exe = new_exe;

        if (exe_size == 0)
            exe[0] = '\0';

        strcat(exe, user);
        strcat(exe, "\n");
        exe_size = strlen(exe);

        if (strstr(user, "?run"))
        {
            Execute("input", exe);
            printf("\n");
        }

        if (strstr(user, "?commit"))
        {
            Execute("input", exe);
            printf("\n");
            exe[0] = '\0';
            exe_size = 0;
        }

        if (strstr(user, "?cancel"))
        {
            exe[0] = '\0';
            exe_size = 0;
        }

        if (strstr(user, "?clear"))
        {
            printf("\033c\033[3J");
        }

        if (strstr(user, "?exit"))
        {
            free(exe);
            return EXIT_SUCCESS;
        }
    }

    free(exe);
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    SetEnvironment(argv[0]);
    if (argc > 1)
        return RunFromFile(argc, argv);
    return Console();
}
