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

/**
 * @brief Run from file using Terminal argument
 * @author Antoine LANDRIEUX
 *
 * @param argc
 * @param argv
 * @return int
 */
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

/**
 * @brief Console mode
 * @author Antoine LANDRIEUX
 *
 * @return int
 */
int Console()
{
    std::string exe = "";

    std::cout
        << "SOARE v" << SOARE_MAJOR << "." << SOARE_MINOR << "." << SOARE_PATCH
        << " [" << __PLATFORM__ << " - Antoine LANDRIEUX (MIT License)]\n"
        << "<https://github.com/AntoineLandrieux/SOARE>\n"
#ifndef __SOARE_NO_COLORED_OUTPUT
        // Lite
        << "\033[2m"
#endif /* __SOARE_NO_COLORED_OUTPUT */
        << "Enter '?run' or '?commit' to run code or '?exit' to quit.\n"
#ifndef __SOARE_NO_COLORED_OUTPUT
        // Normal
        << "\033[0m"
#endif /* __SOARE_NO_COLORED_OUTPUT */
        << std::endl;

    while (true)
    {
        std::cout
#ifndef __SOARE_NO_COLORED_OUTPUT
            // Purple
            << "\033[35m"
#endif /* __SOARE_NO_COLORED_OUTPUT */
            << ">>> "
#ifndef __SOARE_NO_COLORED_OUTPUT
            << "\033[0m"
#endif /* __SOARE_NO_COLORED_OUTPUT */
            ;

        std::string user = "";
        std::getline(std::cin, user);
        exe.append(user.append("\n"));

        if (user.rfind("?run") != std::string::npos)
        {
            SOARE::Execute((char *)"input", const_cast<char *>(exe.c_str()));
            std::cout << std::endl;
        }

        else if (user.rfind("?commit") != std::string::npos)
        {
            SOARE::Execute((char *)"input", const_cast<char *>(exe.c_str()));
            std::cout << std::endl;
            exe = " ";
        }

        else if (user.rfind("?clear") != std::string::npos)
        {
            std::cout << "\033c\033[3J";
        }

        else if (user.rfind("?exit") != std::string::npos)
        {
            return EXIT_SUCCESS;
        }
    }

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    if (argc > 1)
        return RunFromFile(argc, argv);
    return Console();
}
