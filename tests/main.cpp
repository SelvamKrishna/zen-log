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

    zen::log_tag t1 {"APP"};
    zen::log_tag t2 {"SOUND", &t1};
    zen::log_tag t3 {"AUDIO", &t2};

    zen::dbg() << t1 << "Hello World";
    zen::info() << t2 << "Hello World";
    zen::warn() << t3 << "Hello World";
}
