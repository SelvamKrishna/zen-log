#pragma once

#include <array>
#include <format>
#include <string>
#include <iostream>

#include <zen/log/_utils.hpp>

namespace zen {

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
        explicit ansi_256(uint8_t code) : _code {code} {}

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

    template  <typename T>
    concept ansi_type = std::is_same_v<T, ansi_style> || std::is_same_v<T, ansi_color>;

    template  <ansi_type T>
    inline std::ostream& operator << (std::ostream& os, const T& code) noexcept
    {
        if (!is_terminal(os)) return os;
        return os << "\033[" << static_cast<int>(code) << 'm';
    }

    struct _ansi_combo final {
    private:
        static constexpr int INVALID_CODE = -1;
        std::array<int, 4> _codes;

    public:
        constexpr _ansi_combo(int c0) noexcept
            : _codes { c0, INVALID_CODE, INVALID_CODE, INVALID_CODE }
        {}

        template <ansi_type T>
        constexpr _ansi_combo(T val) noexcept
            : _codes { static_cast<int>(val), INVALID_CODE, INVALID_CODE, INVALID_CODE }
        {}

        constexpr _ansi_combo(int c0, int c1) noexcept
            : _codes { c0, c1, INVALID_CODE, INVALID_CODE }
        {}

        template <ansi_type T>
        friend _ansi_combo operator & (_ansi_combo combo, T code)
        {
            for (int& combo_code : combo._codes)
            {
                if (combo_code == INVALID_CODE)
                {
                    combo_code = static_cast<int>(code);
                    break;
                }
            }

            return combo;
        }

        [[nodiscard]] std::string build(bool with_color = true) const noexcept
        {
            if (this->_codes[0] == INVALID_CODE) return "";
            if (!with_color) return "";

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

    template <ansi_type T, ansi_type U>
    inline _ansi_combo operator & (T c0, U c1)
    {
        return _ansi_combo{static_cast<int>(c0), static_cast<int>(c1)};
    }

    inline std::ostream& operator << (std::ostream& os, const _ansi_combo& combo)
    {
        return os << combo.build(is_terminal(os));
    }

    template <typename T>
    concept printable = requires (T text) { std::cout << text; };

    struct ansi_gaurd final {
    private:
        std::ostream& _os;
        const bool _IS_TERMINAL {false};

    public:
        explicit ansi_gaurd(std::ostream& os = std::cout) noexcept
            : _os {os}, _IS_TERMINAL {is_terminal(os)}
        { if (this->_IS_TERMINAL) this->_os << ansi_style::RESET; }

        ~ansi_gaurd() noexcept
        {
            this->_os << ansi_style::RESET << std::endl;
        }

        [[nodiscard]] std::ostream& os() noexcept { return this->_os; }

        template <printable T>
        ansi_gaurd& operator << (const T& data) noexcept
        {
            this->_os << data;
            return *this;
        }
    };


} // namespace zen

#define _FORMATTER_IMPL(_type, ...) \
    template <> \
    struct std::formatter<_type> { \
        constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); } \
        auto format(const _type& type, std::format_context& ctx) const \
        { \
            return std::format_to(ctx.out(), __VA_ARGS__); \
        } \
    }; \

_FORMATTER_IMPL(zen::ansi_style, "\033[{}m", static_cast<int>(type));
_FORMATTER_IMPL(zen::ansi_color, "\033[{}m", static_cast<int>(type));
_FORMATTER_IMPL(zen::_ansi_combo, "{}", type.build());

#undef _FORMATTER_IMPL
