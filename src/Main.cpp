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
        std::ifstream file(filename);

        if (!file)
        {
            SOARE::LeaveException(SOARE::FileError, filename, SOARE::EmptyDocument());
            return EXIT_FAILURE;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        int _error = SOARE::Execute(filename, const_cast<char *>(buffer.str().c_str()));
        errorlevel = _error || errorlevel;
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
    std::string exe = "";

    std::cout

        << "SOARE " << SOARE_VERSION << " [" << __PLATFORM__ << " - Antoine LANDRIEUX (MIT License)]\n"
        << "<https://github.com/AntoineLandrieux/SOARE>\n"

#ifdef __SOARE_COLORED_OUTPUT
        // Lite
        << "\033[2m"
#endif /* __SOARE_COLORED_OUTPUT */

        << "Enter '?run' or '?commit' to run code or '?exit' to quit.\n"

#ifdef __SOARE_COLORED_OUTPUT
        // Normal
        << "\033[0m"
#endif /* __SOARE_COLORED_OUTPUT */

        << std::endl;

    while (true)
    {

        std::cout

#ifdef __SOARE_COLORED_OUTPUT
            // Purple
            << "\033[35m"
#endif /* __SOARE_COLORED_OUTPUT */

            << ">>> "

#ifdef __SOARE_COLORED_OUTPUT
            << "\033[0m"
#endif /* __SOARE_COLORED_OUTPUT */

            ;

        std::string user = "";
        std::getline(std::cin, user);
        exe.append(user.append("\n"));

        if (user.rfind("?run") != std::string::npos)
        {
            SOARE::Execute((char *)"input", const_cast<char *>(exe.c_str()));
            std::cout << std::endl;
        }

        if (user.rfind("?commit") != std::string::npos)
        {
            SOARE::Execute((char *)"input", const_cast<char *>(exe.c_str()));
            std::cout << std::endl;
            exe = " ";
        }

        if (user.rfind("?cancel") != std::string::npos)
        {
            exe = " ";
        }

        if (user.rfind("?clear") != std::string::npos)
        {
            std::cout << "\033c\033[3J";
        }

        if (user.rfind("?exit") != std::string::npos)
        {
            return EXIT_SUCCESS;
        }
    }

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    SOARE::SetEnvironment(argv[0]);
    if (argc > 1)
        return RunFromFile(argc, argv);
    return Console();
}
