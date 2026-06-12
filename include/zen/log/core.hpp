#pragma once

#include <zen/log/_utils.hpp>
#include <zen/log/ansi.hpp>

#include <cstdint>
#include <iostream>
#include <stack>
#include <chrono>

namespace zen {

    /*
        Supported log levels
        - DBUG: For debugging
        - INFO: For informational messages
        - WARN: For warning messages
        - ERRO: For error messagesA
    */
    enum log_lvl : uint8_t { DBG = 0, INFO = 1, WARN = 2, ERR = 3, };

    inline std::ostream& operator << (std::ostream& os, const log_lvl& lvl) noexcept
    {
        static constexpr std::string_view LOG_LVL_LUT[] = {
            "[DBUG]",
            "[INFO]",
            "[WARN]",
            "[ERRO]",
        };

        static constexpr ansi_color ANSI_LUT[] = {
            ansi_color::CYAN,
            ansi_color::GREEN,
            ansi_color::YELLOW,
            ansi_color::RED,
        };

        return os << ANSI_LUT[lvl] << LOG_LVL_LUT[lvl] << ANSI_RESET;
    }

    /*
        Returns a reference to the log stream
        - `lvl`: The log level
        Usage: `zen::log(DBG) << "Hello World";`
    */
    [[nodiscard]] inline std::ostream& log(log_lvl lvl = DBG) noexcept
    {
        return ((lvl == WARN || lvl == ERR) ? std::cerr : std::cout) << '\n' << lvl << " : " ;
    }

    [[nodiscard]] inline std::ostream& dbg()  noexcept { return std::cout << '\n' << DBG  << " : " ; }
    [[nodiscard]] inline std::ostream& info() noexcept { return std::cout << '\n' << INFO << " : " ; }
    [[nodiscard]] inline std::ostream& warn() noexcept { return std::cerr << '\n' << WARN << " : " ; }
    [[nodiscard]] inline std::ostream& err()  noexcept { return std::cerr << '\n' << ERR  << " : " ; }

    /*
        Displayable context tag for logging messages
        - Output: `[<text>]: <Your message>`
    */
    struct log_tag {
    private:
        std::string _text;
        _ansi_combo _combo = ansi_color::WHITE & ansi_style::DIM;
        log_tag*    _from  = nullptr;

    public:
        explicit log_tag(std::string text) noexcept : _text {std::move(text)} {}

        explicit log_tag(std::string text, log_tag* from) noexcept : _text {std::move(text)}, _from {from} {}

        explicit log_tag(std::string text, _ansi_combo combo, log_tag* from = nullptr) noexcept
            : _text  {std::move(text)}
            , _combo {combo}
            , _from  {from}
        {}

        template <ansi_type ANSI>
        explicit log_tag(std::string text, ANSI ansi, log_tag* from = nullptr) noexcept
            : _text {std::move(text)}
            , _combo {static_cast<int>(ansi)}
            , _from {from}
        {}

        friend inline std::ostream& operator << (std::ostream& os, const log_tag& tag) noexcept
        {
            std::stack<const log_tag*> tags;
            for (const log_tag* CURR = &tag; CURR != nullptr; CURR = CURR->_from) tags.push(CURR);

            while (!tags.empty())
            {
                os << tags.top()->_combo << '[' << tags.top()->_text << ']' << ANSI_RESET;
                tags.pop();
            }

            return os << " : ";
        }

        template <typename Err>
            requires std::is_base_of_v<std::exception, Err>
        static inline log_tag err_tag()
        {
            std::string_view _type = _get_type<Err>();
            if (_type.starts_with("std::")) _type.remove_prefix(5);
            if (_type.ends_with("_error")) _type.remove_suffix(6);
            return log_tag {_capitalize(_type), ansi_color::RED};
        }

        static inline log_tag time_tag()
        {
            const std::chrono::time_point NOW = std::chrono::system_clock::now();

            return log_tag {
                std::format("{:%T}", std::chrono::floor<std::chrono::seconds>(NOW)),
                ansi_color::WHITE & ansi_style::DIM
            };
        }
    };

} // namespace zen
