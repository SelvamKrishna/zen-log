#include <zen/log.hpp>

#include <iostream>

int main(void)
{
    zen::ansi_gaurd log_os_gaurd {std::cout};
    zen::ansi_gaurd err_os_gaurd {std::cerr};

    zen::dbg() << "Hello World";
    zen::info() << "Hello World";
    zen::warn() << "Hello World";
    zen::err() << "Hello World";

    zen::debug("test");
    zen::pdebug("test", 1);

    zen::debug(42);
    zen::pdebug(42, 1);
}
