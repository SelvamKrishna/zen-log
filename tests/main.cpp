#include <zen/log.hpp>

int main(void)
{
    zen::logger::init("sample.log");

    zen::info() << "Hello, World!";
    zen::warn() << "Hello, World!";
    zen::err() << "Hello, World!";

    return EXIT_SUCCESS;
}
