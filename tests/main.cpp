#include <zen/log.hpp>

#include <iostream>
#include <cassert>

int main(void)
{
    zen::ansi_gaurd _log_os_gaurd {std::cout};
    zen::ansi_gaurd _err_os_gaurd {std::cerr};

    std::unique_ptr<int> p1 { nullptr };
    std::unique_ptr<int> p2 { new int {42} };

    zen::pdebug(p1, "\n");
    zen::pdebug(p2, "\n");

    zen::err() << zen::log_tag::err_tag<std::runtime_error>() << "Something went wrong\n";

    return EXIT_SUCCESS;
}
