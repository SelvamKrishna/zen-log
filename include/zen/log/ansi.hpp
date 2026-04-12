#pragma once

#include <format>
#include <iostream>

namespace zen {

    enum class ansi_style {
        RESET     = 0,
        BOLD      = 1,
        DIM       = 2,
        ITALIC    = 3,
        UNDERLINE = 4,
        BLINK     = 5,
        REVERSE   = 7,
        HIDDEN    = 8,
        STRIKE    = 9
    };

    inline std::ostream& operator<<(std::ostream& os, const ansi_style& code) noexcept
    {
        return os << "\033[" << static_cast<int>(code) << "m";
    }

    enum class ansi_fg {
        BLACK      = 30,
        RED        = 31,
        GREEN      = 32,
        YELLOW     = 33,
        BLUE       = 34,
        MAGENTA    = 35,
        CYAN       = 36,
        WHITE      = 37,
        EX_BLACK   = 90,
        EX_RED     = 91,
        EX_GREEN   = 92,
        EX_YELLOW  = 93,
        EX_BLUE    = 94,
        EX_MAGENTA = 95,
        EX_CYAN    = 96,
        EX_WHITE   = 97,
    };

    inline std::ostream& operator<<(std::ostream& os, const ansi_fg& code) noexcept
    {
        return os << "\033[" << static_cast<int>(code) << "m";
    }

    enum class ansi_bg {
        BLACK      = 40,
        RED        = 41,
        GREEN      = 42,
        YELLOW     = 43,
        BLUE       = 44,
        MAGENTA    = 45,
        CYAN       = 46,
        WHITE      = 47,
        EX_BLACK   = 100,
        EX_RED     = 101,
        EX_GREEN   = 102,
        EX_YELLOW  = 103,
        EX_BLUE    = 104,
        EX_MAGENTA = 105,
        EX_CYAN    = 106,
        EX_WHITE   = 107,
    };

    inline std::ostream& operator<<(std::ostream& os, const ansi_bg& code) noexcept
    {
        return os << "\033[" << static_cast<int>(code) << "m";
    }

} // namespace zen

template <>
struct std::formatter<::zen::ansi_style> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    auto format(const ::zen::ansi_style &code, std::format_context &ctx) const
    {
        return std::format_to(ctx.out(), "\033[{}m", static_cast<int>(code));
    }
};

template <>
struct std::formatter<::zen::ansi_fg> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    auto format(const ::zen::ansi_fg &code, std::format_context &ctx) const
    {
        return std::format_to(ctx.out(), "\033[{}m", static_cast<int>(code));
    }
};

template <>
struct std::formatter<::zen::ansi_bg> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    auto format(const ::zen::ansi_bg &code, std::format_context &ctx) const
    {
        return std::format_to(ctx.out(), "\033[{}m", static_cast<int>(code));
    }
};
