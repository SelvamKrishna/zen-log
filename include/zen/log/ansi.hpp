#pragma once

#include <array>
#include <format>
#include <iostream>
#include <string>

namespace zen {

    inline static constexpr std::string_view ANSI_RESET = "\033[0m";

    struct ansi_gaurd final {
    private:
        std::ostream& _os;

    public:
        explicit ansi_gaurd(std::ostream& os = std::cout) noexcept : _os {os} { this->_os << ANSI_RESET; }
        ~ansi_gaurd() noexcept { this->_os << ANSI_RESET; }
    };

    struct ansi_rgb final {
    private:
        uint8_t _r, _g, _b;

    public:
        explicit ansi_rgb(uint8_t r, uint8_t g, uint8_t b) : _r {r}, _g {g}, _b {b} {}

        [[nodiscard]] std::string fg() const noexcept
        {
            return std::format("\033[38;2;{};{};{}m", this->_r, this->_g, this->_b);
        }

        [[nodiscard]] std::string bg() const noexcept
        {
            return std::format("\033[48;2;{};{};{}m", this->_r, this->_g, this->_b);
        }
    };

    struct ansi_256 final {
    private:
        uint8_t _code;

    public:
        explicit ansi_256(uint8_t code) : _code(code) {}

        [[nodiscard]] std::string fg() const noexcept { return std::format("\033[38;5;{}m", this->_code); }
        [[nodiscard]] std::string bg() const noexcept { return std::format("\033[48;5;{}m", this->_code); }
    };

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

    inline std::ostream& operator << (std::ostream& os, const ansi_style& code) noexcept
    {
        return os << "\033[" << static_cast<int>(code) << "m";
    }

    enum class ansi_color {
        BLACK         = 30,
        RED           = 31,
        GREEN         = 32,
        YELLOW        = 33,
        BLUE          = 34,
        MAGENTA       = 35,
        CYAN          = 36,
        WHITE         = 37,
        BG_BLACK      = 40,
        BG_RED        = 41,
        BG_GREEN      = 42,
        BG_YELLOW     = 43,
        BG_BLUE       = 44,
        BG_MAGENTA    = 45,
        BG_CYAN       = 46,
        BG_WHITE      = 47,
        EX_BLACK      = 90,
        EX_RED        = 91,
        EX_GREEN      = 92,
        EX_YELLOW     = 93,
        EX_BLUE       = 94,
        EX_MAGENTA    = 95,
        EX_CYAN       = 96,
        EX_WHITE      = 97,
        BG_EX_BLACK   = 100,
        BG_EX_RED     = 101,
        BG_EX_GREEN   = 102,
        BG_EX_YELLOW  = 103,
        BG_EX_BLUE    = 104,
        BG_EX_MAGENTA = 105,
        BG_EX_CYAN    = 106,
        BG_EX_WHITE   = 107,
    };

    inline std::ostream& operator << (std::ostream& os, const ansi_color& code) noexcept
    {
        return os << "\033[" << static_cast<int>(code) << "m";
    }

    template  <typename T>
    concept ansi_support =
        std::is_same_v<T, ansi_style> ||
        std::is_same_v<T, ansi_color> ||
        std::is_same_v<T, int>
    ;

    struct _ansi_combo final {
    private:
        static constexpr int INVALID_CODE = -1;
        std::array<int, 8> _codes;

    public:
        explicit constexpr _ansi_combo(int c0, int c1) noexcept
        : _codes {
            c0          , c1          , INVALID_CODE, INVALID_CODE,
            INVALID_CODE, INVALID_CODE, INVALID_CODE, INVALID_CODE
        } {}

        template <ansi_support T>
        friend _ansi_combo operator & (_ansi_combo combo, T code)
        {
            for (int& c : combo._codes)
            {
                if (c == INVALID_CODE)
                {
                    c = static_cast<int>(code);
                    break;
                }
            }

            return combo;
        }

        [[nodiscard]] std::string build() const noexcept
        {
            if (this->_codes[0] == INVALID_CODE) return "";

            std::string result = "\033[" + std::to_string(this->_codes[0]);

            for (size_t i = 1; i < this->_codes.size(); ++i)
            {
                int code = this->_codes[i];
                if (code == INVALID_CODE) break;
                result += ';' + std::to_string(code);
            }

            return result + 'm';
        }
    };

    template <ansi_support ANSI_T, ansi_support ANSI_U>
    inline _ansi_combo operator & (ANSI_T c0, ANSI_U c1)
    {
        return _ansi_combo{static_cast<int>(c0), static_cast<int>(c1)};
    }

    inline std::ostream& operator << (std::ostream& os, const _ansi_combo& combo)
    {
        return os << combo.build();
    }

} // namespace zen

template <>
struct std::formatter<::zen::ansi_style> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const ::zen::ansi_style& code, std::format_context& ctx) const
    {
        return std::format_to(ctx.out(), "\033[{}m", static_cast<int>(code));
    }
};

template <>
struct std::formatter<::zen::ansi_color> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const ::zen::ansi_color& code, std::format_context& ctx) const
    {
        return std::format_to(ctx.out(), "\033[{}m", static_cast<int>(code));
    }
};

template <>
struct std::formatter<::zen::_ansi_combo> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const ::zen::_ansi_combo& combo, std::format_context& ctx) const
    {
        return std::format_to(ctx.out(), "{}", combo.build());
    }
};
