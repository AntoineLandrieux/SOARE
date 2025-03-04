#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

/**
 *  _____  _____  ___  ______ _____
 * /  ___||  _  |/ _ \ | ___ \  ___|
 * \ `--. | | | / /_\ \| |_/ / |__
 *  `--. \| | | |  _  ||    /|  __|
 * /\__/ /\ \_/ / | | || |\ \| |___
 * \____/  \___/\_| |_/\_| \_\____/
 *
 * Antoine LANDRIEUX (MIT License) <Main.cpp>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

#include <SOARE/SOARE.h>

namespace App
{

    bool CONSOLE = false;

    int RunFromFile(int argc, char *argv[])
    {
        for (int i = 1; i < argc; i++)
        {
            char *filename = argv[i];
            std::ifstream file(filename);

            if (!file)
            {
                SOARE::LeaveException(SOARE::FileError, filename, SOARE::EmptyDocument());
                return EXIT_FAILURE;
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            SOARE::Execute(filename, const_cast<char *>(buffer.str().c_str()));
        }

        return EXIT_SUCCESS;
    }

    int Console()
    {
        CONSOLE = true;
        std::string exe = "";
        std::string file = __SOARE_FILE__;

        std::cout
#ifndef __SOARE_NO_COLORED_OUTPUT
            // Blue
            << "\033[1;34m"
#endif /* __SOARE_NO_COLORED_OUTPUT */
            << "SOARE v" << SOARE_MAJOR << "." << SOARE_MINOR << "." << SOARE_PATCH
            << " Antoine LANDRIEUX (MIT License)\n"
            << "<https://github.com/AntoineLandrieux/SOARE>\n"
#ifndef __SOARE_NO_COLORED_OUTPUT
            // Normal
            << "\033[0m"
#endif /* __SOARE_NO_COLORED_OUTPUT */
            << "Enter '?run' or '?commit' to run code or '?exit' to quit.\n"
            << std::endl;

        while (true)
        {
            std::cout
#ifndef __SOARE_NO_COLORED_OUTPUT
                // Purple
                << "\033[0;35m"
#endif /* __SOARE_NO_COLORED_OUTPUT */
                << ">>> "
#ifndef __SOARE_NO_COLORED_OUTPUT
                << "\033[0m"
#endif /* __SOARE_NO_COLORED_OUTPUT */
                ;

            std::string user = "";
            std::getline(std::cin, user);

            if (user[0] == '?')
            {
                if (user == "?run" || user == "?commit")
                    SOARE::Execute(const_cast<char *>(file.c_str()), const_cast<char *>(exe.c_str()));

                if (user == "?commit")
                    exe = " ";

                else if (user == "?exit")
                    return EXIT_SUCCESS;

                else if (user == "?clear")
                    std::cout << "\033c\033[3J";

                continue;
            }

            exe.append(user.append("\n"));
        }

        return EXIT_SUCCESS;
    }

}

int main(int argc, char *argv[])
{
    if (argc > 1)
        return App::RunFromFile(argc, argv);
    return App::Console();
}

static void __attribute__((destructor)) kill(void)
{
    if (App::CONSOLE)
        std::cerr << "\nBye!\n";
}
