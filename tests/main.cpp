#include <zen/log.hpp>

#include <iostream>

int main(void)
{
    zen::ansi_gaurd _log_os_gaurd {std::cout};
    zen::ansi_gaurd _err_os_gaurd {std::cerr};
    std::cout << (zen::ansi_style::BOLD & zen::ansi_style::UNDERLINE & zen::ansi_color::RED) << "Hello";
    return 0;
}
