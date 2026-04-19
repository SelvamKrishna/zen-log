#include <zen/log.hpp>

#include <iostream>
#include <unordered_map>
#include <vector>

int main(void)
{
    zen::ansi_gaurd log_os_gaurd {std::cout};
    zen::ansi_gaurd err_os_gaurd {std::cerr};

    std::string name = "Alice";

    std::vector<int> vec {1, 2, 3, 4, 5};

    std::vector<std::vector<int>> vec2 {
        {1, 2, 3},
        {4, 5, 6},
    };

    std::unordered_map<std::string, std::vector<int>> map {
        {"alice", {1, 2, 3} },
        {"bob"  , {4, 5, 6} },
    };

    std::unordered_map<std::string, int> map2 {
        {"alice", 1},
        {"bob"  , 4},
    };

    ZEN_VAR(name);
    ZEN_VAR_P(name);
    std::cout << "\n\n=================\n";
    ZEN_VAR(vec);
    ZEN_VAR_P(vec);
    std::cout << "\n\n=================\n";
    ZEN_VAR(vec2);
    ZEN_VAR_P(vec2);
    std::cout << "\n\n=================\n";
    ZEN_VAR(map);
    ZEN_VAR_P(map);
    std::cout << "\n\n=================\n";
    ZEN_VAR(map2);
    ZEN_VAR_P(map2);
    std::cout << "\n\n=================\n";

    return 0;
}
