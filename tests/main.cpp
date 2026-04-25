#include <zen/log.hpp>

#include <iostream>

int main(void)
{
    zen::ansi_gaurd log_os_gaurd {std::cout};
    zen::ansi_gaurd err_os_gaurd {std::cerr};

    zen::log(zen::DBG) << "Hello World";
    zen::log(zen::INFO) << "Hello World";
    zen::log(zen::WARN) << "Hello World";
    zen::log(zen::ERR) << "Hello World";

    zen::dbg() << "Hello World";
    zen::info() << "Hello World";
    zen::warn() << "Hello World";
    zen::err() << "Hello World";
}
