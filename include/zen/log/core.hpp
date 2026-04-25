#pragma once

#include <zen/log/ansi.hpp>

#include <cstdint>
#include <iostream>

namespace zen {

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

    [[nodiscard]] inline std::ostream& log(log_lvl lvl = DBG) noexcept
    {
        return ((lvl == WARN || lvl == ERR) ? std::cerr : std::cout) << '\n' << lvl << " : " ;
    }

    [[nodiscard]] inline std::ostream& dbg()  noexcept { return std::cout << '\n' << DBG  << " : " ; }
    [[nodiscard]] inline std::ostream& info() noexcept { return std::cout << '\n' << INFO << " : " ; }
    [[nodiscard]] inline std::ostream& warn() noexcept { return std::cerr << '\n' << WARN << " : " ; }
    [[nodiscard]] inline std::ostream& err()  noexcept { return std::cerr << '\n' << ERR  << " : " ; }

} // namespace zen
