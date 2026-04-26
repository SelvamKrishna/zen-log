#include <zen/log.hpp>

#include <iostream>

int main(void)
{
    zen::ansi_gaurd log_os_gaurd {std::cout};
    zen::ansi_gaurd err_os_gaurd {std::cerr};

    std::vector<std::vector<int>> samepleVec = {
        {1, 2, 3, 4, 5, 6},
        {1, 2, 3, 4, 5, 6},
        {1, 2, 3, 4, 5, 6},
        {1, 2, 3, 4, 5, 6},
    };

    ZEN_VAR(samepleVec);
    ZEN_VAR_P(samepleVec);
}
