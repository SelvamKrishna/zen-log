#include <zen/log.hpp>

#include <fstream>

int main(void)
{
    static std::ofstream file {"test.log", std::ios::trunc | std::ios::out};

    if (!file.is_open())
        return EXIT_FAILURE;

    zen::logger::init(file, file);

    zen::info() << "Hello, World!";
    zen::warn() << "Hello, World!";
    zen::err() << "Hello, World!";

    return EXIT_SUCCESS;
}
