#include <zen/log.hpp>

#include <iostream>

int main(void)
{
    zen::ansi_gaurd log_os_gaurd {std::cout};
    zen::ansi_gaurd err_os_gaurd {std::cerr};

    std::cout << zen::ansi_style::BOLD << "This is bold text" << std::endl;
    std::cout << zen::ansi_color::RED << "This is red text" << std::endl;
    std::cout << (zen::ansi_style::UNDERLINE & zen::ansi_color::BLUE) << "This is underlined blue text" << std::endl;

    return 0;
}
