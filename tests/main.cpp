#include <zen/log.hpp>

#include <iostream>

int main(void)
{
    std::cout << zen::ansi_fg::BLUE << "Hello, World" << zen::ansi_style::RESET;
    return 0;
}
