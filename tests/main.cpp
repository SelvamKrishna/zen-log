#include <zen/log.hpp>

#include <iostream>

struct vec2
{
    float x, y;
};

template <>
struct zen::dbg_trait<vec2>
{
    static void debug(const vec2& data) noexcept { std::cout << '[' << data.x << ", " << data.y << ']'; }
    static void pdebug(const vec2& data, int indent = 0) noexcept { (void)indent; dbg_trait<vec2>::debug(data); }
};

int main(void)
{
    zen::ansi_gaurd log_os_gaurd {std::cout};
    zen::ansi_gaurd err_os_gaurd {std::cerr};

    zen::dbg() << "Hello World";
    zen::info() << "Hello World";
    zen::warn() << "Hello World";
    zen::err() << "Hello World";

    std::unique_ptr<vec2> v { new vec2 {1.0f, 2.0f} };
    zen::debugn(v);
    zen::pdebugn(v, 1);

    std::unique_ptr<std::string> ptr { new std::string {"hello"} };
    zen::debugn(ptr);
    zen::pdebugn(ptr, 1);

    std::optional<std::string> opt { "hello" };
    zen::debugn(opt);
    zen::pdebugn(opt, 1);

    std::variant<std::string, float> var { "hello" };
    zen::debugn(var);
    zen::pdebugn(var, 1);
}
