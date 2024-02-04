#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SOARE/SOARE.h>

static void CMD()
{
    fprintf(stdout,
            "SOARE v%d.%d.%d %s Antoine LANDRIEUX\n%s\n",
            SOARE_VERSION_MAJOR,
            SOARE_VERSION_MINOR,
            SOARE_VERSION_PATCH,
            __DATE__,
            "(https://github.com/AntoineLandrieux/SOARE)\n");

    char user[SOARE_MAX_INPUT];
    TOKENS tokens = NULL;

    while (1)
    {
        fprintf(stdout, "> ");

        if (fgets(user, sizeof(user), stdin) == NULL)
            break;
        else if (!strcmp(user, "\n"))
        {
            resultlang_free(ExecuteTokens(tokens));
            tokens = NULL;
        }
        else
            tokens = token_join(tokens, Tokenizer(user));
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        CMD();
    else
        resultlang_free(ExecuteTokens(file_read(argv[1])));
    return EXIT_SUCCESS;
}
