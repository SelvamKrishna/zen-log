#pragma once

#include <zen/log/_utils.hpp>
#include <zen/log/ansi.hpp>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <string_view>
#include <source_location>

namespace zen {

    // null stream buffer for a null std::ostream
    struct _null_buf final : public std::streambuf {
    protected:
        int overflow(int c) { return c; }
    };

    // Supported log levels
    enum log_lvl : uint8_t { DBG = 0, INFO = 1, WARN = 2, ERR = 3, };

    inline std::ostream& operator << (std::ostream& os, const log_lvl& lvl) noexcept
    {
        static constexpr std::string_view LOG_LVL_LUT[] = {
            "[DEBUG]",
            "[INFO]",
            "[WARN]",
            "[ERROR]",
        };

        if (!is_terminal(os)) return os << LOG_LVL_LUT[lvl];

        static constexpr ansi_color ANSI_LUT[] = {
            ansi_color::CYAN,
            ansi_color::GREEN,
            ansi_color::YELLOW,
            ansi_color::RED,
        };

        return os
            << ansi_style::RESET << ANSI_LUT[lvl]
            << ansi_style::BOLD << LOG_LVL_LUT[lvl]
            << ansi_style::RESET;
    }

    /*
        Singleton logger class
        - handles log level filtering
        - handles output streams preparation and cleanup
    */
    struct logger final {
    private:
        std::ostream* _out_stream;
        std::ostream* _err_stream;

        ansi_gaurd _out_os;
        ansi_gaurd _err_os;

        _null_buf    _null_buf {};
        std::ostream _null_stream {&_null_buf};
        ansi_gaurd   _null_os {_null_stream};

        std::atomic<log_lvl> _min_lvl {DBG};
        bool _flag_log_time {false};
        bool _owns_stream {false};

        static inline logger* _instance = nullptr;
        static inline std::once_flag _init_flag;

        explicit logger(std::ostream& out_os, std::ostream& err_os, bool owns = false)
            : _out_stream(&out_os)
            , _err_stream(&err_os)
            , _out_os(out_os)
            , _err_os(err_os)
            , _owns_stream(owns)
        {}

    public:
        logger(const logger&) = delete;
        logger& operator=(const logger&) = delete;

        ~logger()
        {
            if (this->_owns_stream)
            {
                delete _out_stream;
                delete _err_stream;
            }
        }

        static void set_min_level(log_lvl lvl)
        {
            logger::instance()._min_lvl.store(lvl, std::memory_order_relaxed);
        }

        static void set_timestamp_flag(bool flag) { logger::instance()._flag_log_time = flag; }

        static void init(
            std::ostream& out_os = std::cout,
            std::ostream& err_os = std::cerr,
            log_lvl min_lvl = DBG,
            bool flag_log_time = false
        ) {
            std::call_once(logger::_init_flag, [&]() {
                static logger instance {out_os, err_os, false};
                logger::_instance = &instance;
            });

            logger& inst = logger::instance();
            inst._min_lvl.store(min_lvl, std::memory_order_relaxed);
            inst._flag_log_time = flag_log_time;
        }

        static void init(
            std::string file_path,
            log_lvl min_lvl = DBG,
            bool flag_log_time = false
        )
        {
            std::ofstream* log_file = new std::ofstream{file_path};

            if (!log_file->good()) [[unlikely]]
            {
                delete log_file;
                throw std::runtime_error("Failed to open log file.");
            }

            logger::init(*log_file, *log_file, min_lvl, flag_log_time);
        }

        [[nodiscard]] static inline logger& instance()
        {
            if (!logger::_instance) throw std::runtime_error(
                "Logger not initialized. Call zen::logger::init() first."
            );

            return *logger::_instance;
        }

        [[nodiscard]] static inline ansi_gaurd& log(const log_lvl& lvl) {
            logger& inst = logger::instance();

            if (lvl < inst._min_lvl.load(std::memory_order_relaxed)) return inst._null_os;

            ansi_gaurd& os = (lvl == ERR || lvl == WARN)
                ? inst._err_os
                : inst._out_os;

            os << '\n';

            if (inst._flag_log_time)
            {
                const std::chrono::time_point NOW = std::chrono::system_clock::now();

                os  << _ansi_combo {ansi_color::WHITE & ansi_style::DIM}
                    << std::format("{:%T}", std::chrono::floor<std::chrono::seconds>(NOW))
                    << ansi_style::RESET << ' ';
            }

            os << lvl << ": ";
            return os;
        }

        [[nodiscard]] static std::ostream& out_stream() { return *instance()._out_stream; }
        [[nodiscard]] static std::ostream& err_stream() { return *instance()._err_stream; }
    };

    [[nodiscard]] inline ansi_gaurd& dbg()  noexcept { return logger::log(DBG); }
    [[nodiscard]] inline ansi_gaurd& info() noexcept { return logger::log(INFO); }
    [[nodiscard]] inline ansi_gaurd& warn() noexcept { return logger::log(WARN); }
    [[nodiscard]] inline ansi_gaurd& err()  noexcept { return logger::log(ERR); }

    /*
        Displayable context tag for logging messages
        - Output: `[<Tag name>]: <Your message>`
    */
    struct log_tag {
    private:
        std::string    _text;
        _ansi_combo    _combo {ansi_color::WHITE & ansi_style::DIM};
        const log_tag* _FROM  {nullptr};

        mutable std::string _cached_tag;
        mutable bool        _is_cached {false};
        mutable bool        _was_ansi {false};

        void _ensure_cache(bool with_ansi = true) const
        {
            if (this->_was_ansi != with_ansi) this->_is_cached = false;
            if (this->_is_cached) return;

            this->_cached_tag.clear();
            this->_was_ansi = with_ansi;

            const log_tag* TAGS[16];
            size_t count {0};

            for (const log_tag* CURR = this; CURR != nullptr && count < 16; CURR = CURR->_FROM)
                TAGS[count++] = CURR;

            if (with_ansi) for (size_t i {count}; i > 0; --i)
                this->_cached_tag += std::format(
                    "{}[{}]{}",
                    TAGS[i - 1]->_combo, TAGS[i - 1]->_text, ansi_style::RESET
                );
            else for (size_t i {count}; i > 0; --i)
                this->_cached_tag += '[' + TAGS[i - 1]->_text + ']';

            this->_is_cached = true;
            this->_cached_tag += ": ";
        }

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

        void set_text(std::string text) noexcept
        {
            this->_text = std::move(text);
            this->_is_cached = false;
        }

        void set_from(const log_tag* from) noexcept
        {
            this->_FROM = from;
            this->_is_cached = false;
        }

        friend inline std::ostream& operator << (std::ostream& os, const log_tag& tag) noexcept
        {
            tag._ensure_cache(is_terminal(os));
            return os << tag._cached_tag;
        }

        template <typename Err>
            requires std::is_base_of_v<std::exception, Err>
        [[nodiscard]] static inline log_tag err_tag()
        {
            std::string_view _type = _get_type<Err>();
            if (_type.starts_with("std::")) _type.remove_prefix(5);
            if (_type.ends_with("_error")) _type.remove_suffix(6);
            return log_tag {_capitalize(_type), ansi_color::RED};
        }

        [[nodiscard]] static inline log_tag time_tag()
        {
            const std::chrono::time_point NOW = std::chrono::system_clock::now();

            return log_tag {
                std::format("{:%T}", std::chrono::floor<std::chrono::seconds>(NOW)),
                ansi_color::WHITE & ansi_style::DIM
            };
        }

        [[nodiscard]] static inline log_tag source_location_tag(std::source_location loc)
        {
            return log_tag {
                std::format("{}:{}", loc.file_name(), loc.line()),
                ansi_color::WHITE & ansi_style::DIM
            };
        }

        [[nodiscard]] inline constexpr ansi_gaurd& log(log_lvl lvl)  const noexcept
        {
            return logger::log(lvl) << (*this);
        }

        [[nodiscard]] inline constexpr ansi_gaurd& dbg()  const noexcept { return zen::dbg()  << (*this); }
        [[nodiscard]] inline constexpr ansi_gaurd& info() const noexcept { return zen::info() << (*this); }
        [[nodiscard]] inline constexpr ansi_gaurd& warn() const noexcept { return zen::warn() << (*this); }
        [[nodiscard]] inline constexpr ansi_gaurd& err()  const noexcept { return zen::err()  << (*this); }
    };

} // namespace zen
