#pragma once

#include <zen/log/core.hpp>

#include <chrono>
#include <format>
#include <iostream>
#include <source_location>
#include <string>
#include <string_view>
#include <thread>

namespace zen {

    #define _LOC \
        const std::source_location& loc = std::source_location::current()

    // Formatted string wrapper with built-in type support
    struct fmt_str final {
    private:
        std::string _text;

    public:
        template <typename... Args>
        constexpr fmt_str(std::format_string<Args...> str, Args&&... args)
            : _text{std::format(str, std::forward<Args>(args)...)}
        {}

        constexpr fmt_str(const char* str) : _text(str) {}
        constexpr fmt_str(char* str) : _text(str) {}
        constexpr fmt_str(const std::string& str) : _text(str) {}
        constexpr fmt_str(std::string&& str) : _text(std::move(str)) {}
        constexpr fmt_str(std::string_view str) : _text(str) {}
        constexpr fmt_str(char c) : _text(1, c) {}

        operator std::string() const { return _text; }
        operator std::string_view() const { return _text; }
        operator const char*() const { return _text.c_str(); }

        [[nodiscard]] const std::string& str() const { return _text; }
        [[nodiscard]] const char* c_str() const { return _text.c_str(); }
        [[nodiscard]] size_t size() const { return _text.size(); }
        [[nodiscard]] bool empty() const { return _text.empty(); }

        friend std::ostream& operator << (std::ostream& os, const fmt_str& str) { return os << str._text; }
    };

    // Helper function for automatic deduction
    template <typename... Args>
    inline fmt_str fstr(std::format_string<Args...> fmt, Args&&... args)
    {
        return fmt_str(fmt, std::forward<Args>(args)...);
    }

    // Abort's process with provided message
    [[noreturn]]
    inline void panic(const fmt_str& msg, const log_tag* from = nullptr, _LOC)
    {
        static log_tag panic_tag {"PANIC", ansi_color::BG_EX_RED & ansi_style::BOLD};
        panic_tag.set_from(from);
        const std::chrono::time_point NOW = std::chrono::system_clock::now();

        static log_tag time_tag {log_tag::time_tag()};
        time_tag.set_text(std::format("{:%T}", std::chrono::floor<std::chrono::seconds>(NOW)));
        time_tag.set_from(&panic_tag);

        static log_tag loc_tag {log_tag::source_location_tag(loc)};
        loc_tag.set_text(std::format("{}:{}", loc.file_name(), loc.line()));
        loc_tag.set_from(&time_tag);

        static log_tag thread_tag {"", ansi_color::WHITE & ansi_style::DIM};
        thread_tag.set_text(std::format("T#{}", std::this_thread::get_id()));
        thread_tag.set_from(&loc_tag);

        static const log_tag& TAG {thread_tag};
        std::cerr << "\n---\n" << TAG << msg << std::endl;
        std::abort();
    }

    // Panic if condition is true
    inline void panic_if(bool cnd, const fmt_str& msg, const log_tag* from = nullptr, _LOC)
    { if (cnd) panic(msg, from, loc); }

    // Utility struct to handle logging processes
    struct log_process final {
    private:
        std::string _process_desc;
        bool _is_active {true};
        const log_tag _TAG;

        static const inline log_tag _FAIL_TAG {"FAIL", ansi_color::RED};

        [[nodiscard]] ansi_gaurd& _log_desc(log_lvl lvl = log_lvl::DBG) const noexcept
        {
            return this->_TAG.log(lvl)
                << ansi_style::BOLD << this->_process_desc << ansi_style::RESET << ' ';
        }

    public:
        // Logs start of process (if ZEN_TRACE flag is defined)
        explicit log_process(const fmt_str&& message, const log_tag* from = nullptr) noexcept
            : _process_desc {std::move(message.str())}
            , _TAG {">>", ansi_color::MAGENTA & ansi_style::BOLD, from}
        {
            #ifdef ZEN_TRACE
            this->_log_desc()
                << ansi_style::DIM << ansi_style::ITALIC << "Started" << ansi_style::RESET;
            #endif
        }

        // Logs end of process (if ZEN_TRACE flag is defined)
        ~log_process()
        {
            #ifdef ZEN_TRACE
            this->_log_desc()
                << ansi_style::DIM << ansi_style::ITALIC << "Finished" << ansi_style::RESET;
            #endif
        }

        // Logs success report of process
        void log_success()
        {
            if (!this->_is_active) return;
            this->_log_desc() << ansi_color::GREEN << "OK" << ansi_style::RESET;
            this->_is_active = false;
        }

        // Logs failure report of process
        void log_failure(const fmt_str& msg)
        {
            if (!this->_is_active) return;
            this->_log_desc(ERR) << log_process::_FAIL_TAG << msg;
            this->_is_active = false;
        }

        // Logs warning report of process (process is still active)
        void log_fallback(const fmt_str& msg) { if (this->_is_active) this->_log_desc(WARN) << msg; }

        // Logs panic report of process (panics if process is still active)
        void log_panic(const fmt_str& msg, _LOC) { panic_if(this->_is_active, msg, &this->_TAG, loc); }

        // Logs conditional panic report of process (panics if process is still active)
        void log_panic_if(bool cnd, const fmt_str& msg, _LOC) { if (cnd) this->log_panic(msg, loc); }
    };

    #undef _LOC

} // namespace zen
