#include <zen/log.hpp>

#include <iostream>

int main(void)
{
    zen::ansi_gaurd _log_os_gaurd {std::cout};
    zen::ansi_gaurd _err_os_gaurd {std::cerr};

    auto a = zen::ansi_rgb(255, 128, 0);

    std::cout << a.bg() << "Hello" << zen::ANSI_RESET << a.fg() << " World" ;

    return 0;
}
