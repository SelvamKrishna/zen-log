#include <zen/log.hpp>

#include <iostream>
#include <cassert>

int main(void)
{
    zen::ansi_gaurd _log_os_gaurd {std::cout};
    zen::ansi_gaurd _err_os_gaurd {std::cerr};

    const zen::log_tag INPUT {"INPUT", zen::ansi_color::MAGENTA};

    while (true)
    {
        std::string s = "";
        std::getline(std::cin, s);
        if (s == "exit") break;

        zen::info() << zen::log_tag::time_tag() << s << "\n" << INPUT;
    }

    return EXIT_SUCCESS;
}
