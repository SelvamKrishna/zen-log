#pragma once

#include <zen/log/_utils.hpp>
#include <zen/log/ansi.hpp>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <stack>

namespace zen {

    // null stream buffer for a null std::ostream
    struct _null_buf final : public std::streambuf {
    protected:
        int overflow(int c) { return c; }
    };

    /*
        Supported log levels
        - DBG: For debugging
        - INFO: For informational messages
        - WARN: For warning messages
        - ERR: For error messagesA
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

        return os << ANSI_LUT[lvl] << ansi_style::BOLD << LOG_LVL_LUT[lvl] << ANSI_RESET;
    }

    /*
        Singleton logger class
        - handles log level filtering
        - handles output streams preparation and cleanup
    */
    struct logger final {
    private:
        ansi_gaurd   _out_os;
        ansi_gaurd   _err_os;
        std::ostream _null_os;
        _null_buf    _null_buf {};

        log_lvl _min_lvl {DBG};
        bool _flag_log_time {false};

        static inline logger* _instance = nullptr;
        static inline std::once_flag _init_flag;

        explicit logger(std::ostream& out_os, std::ostream& err_os)
            : _out_os  {out_os}
            , _err_os  {err_os}
            , _null_os {&_null_buf}
        {}

    public:
        logger(const logger&) = delete;
        logger& operator=(const logger&) = delete;

        constexpr static void set_min_level(const log_lvl& lvl) { logger::instance()._min_lvl = lvl; }
        constexpr static void set_timestamp_flag(bool flag) { logger::instance()._flag_log_time = flag; }

        static void init(
            std::ostream& out_os = std::cout,
            std::ostream& err_os = std::cerr,
            log_lvl min_lvl = DBG,
            bool flag_log_time = false
        )
        {
            std::call_once(logger::_init_flag, [&]() {
                static logger instance {out_os, err_os};
                logger::_instance = &instance;
            });

            logger::instance()._min_lvl = min_lvl;
            logger::instance()._flag_log_time = flag_log_time;
        }

        [[nodiscard]] static inline logger& instance()
        {
            if (!logger::_instance) throw
                std::runtime_error("Logger not initialized. Call zen::logger::init() first.");

            return *logger::_instance;
        }

        [[nodiscard]] static inline std::ostream& log(const log_lvl& lvl)
        {
            logger& inst = logger::instance();

            if (lvl < inst._min_lvl) return _instance->_null_os;

            return ((lvl == ERR || lvl == WARN) ? inst._err_os: inst._err_os)
                .os() << '\n' << lvl << " : ";
        }
    };

    [[nodiscard]] inline std::ostream& dbg()  noexcept { return logger::log(DBG); }
    [[nodiscard]] inline std::ostream& info() noexcept { return logger::log(INFO); }
    [[nodiscard]] inline std::ostream& warn() noexcept { return logger::log(WARN); }
    [[nodiscard]] inline std::ostream& err()  noexcept { return logger::log(ERR); }

    /*
        Displayable context tag for logging messages
        - Output: `[<Tag name>]: <Your message>`
    */
    struct log_tag {
    private:
        std::string    _text;
        _ansi_combo    _combo {ansi_color::WHITE & ansi_style::DIM};
        const log_tag* _FROM  {nullptr};

    public:
        explicit log_tag(std::string text) noexcept : _text {std::move(text)} {}

        explicit log_tag(std::string text, const log_tag* from) noexcept : _text {std::move(text)}, _FROM {from} {}

        explicit log_tag(std::string text, _ansi_combo combo, const log_tag* from = nullptr) noexcept
            : _text  {std::move(text)}
            , _combo {combo}
            , _FROM  {from}
        {}

        template <ansi_type ANSI>
        explicit log_tag(std::string text, ANSI ansi, log_tag* from = nullptr) noexcept
            : _text {std::move(text)}
            , _combo {static_cast<int>(ansi)}
            , _FROM {from}
        {}

        friend inline std::ostream& operator << (std::ostream& os, const log_tag& tag) noexcept
        {
            std::stack<const log_tag*> tags;
            for (const log_tag* CURR = &tag; CURR != nullptr; CURR = CURR->_FROM) tags.push(CURR);

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

        [[nodiscard]] inline constexpr std::ostream& dbg()  const noexcept { return zen::dbg()  << (*this); }
        [[nodiscard]] inline constexpr std::ostream& info() const noexcept { return zen::info() << (*this); }
        [[nodiscard]] inline constexpr std::ostream& warn() const noexcept { return zen::warn() << (*this); }
        [[nodiscard]] inline constexpr std::ostream& err()  const noexcept { return zen::err()  << (*this); }
    };

    // panic's and aborts the program if the condition is false
    inline void assert(bool condition, std::string_view message)
    {
        static const log_tag ASSERT_TAG {"ASSERT", ansi_color::BG_EX_RED};
        if (condition) return;

        err() << ASSERT_TAG << message << std::endl;
        std::abort();
    }

    // panic's and aborts the program if the condition is false
    inline void assert(bool condition, std::string_view message, const log_tag& tag)
    {
        static const log_tag ASSERT_TAG {"[ASSERT]", ansi_color::BG_EX_RED};
        if (condition) return;

        err() << ASSERT_TAG << tag << message << std::endl;
        std::abort();
    }

} // namespace zen
