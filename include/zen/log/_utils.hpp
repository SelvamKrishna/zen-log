#pragma once

#include <iostream>
#include <string>
#include <string_view>

#ifdef _WIN32
#include <io.h>
#define _IS_OS_TERM _isatty
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#else
#include <unistd.h>
#define _IS_OS_TERM isatty
#endif

namespace zen {

    [[nodiscard]] constexpr inline bool is_terminal(const std::ostream& os) noexcept
    {
        if (os.rdbuf() == std::cout.rdbuf())
            return _IS_OS_TERM(STDOUT_FILENO) != 0;

        if (os.rdbuf() == std::cerr.rdbuf())
            return _IS_OS_TERM(STDERR_FILENO) != 0;

        return false;
    }

    inline constexpr std::string _capitalize(std::string_view sv) noexcept {
        std::string text {sv};
        for (char& c : text) if (c >= 'a' && c <= 'z') c -= ('a' - 'A');
        return text;
    }

    inline constexpr std::string_view _trim(std::string_view sv) noexcept {
        constexpr auto IS_SPACE = [](char c) constexpr -> bool {
            return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
        };

        while (!sv.empty() && IS_SPACE(sv.front())) sv.remove_prefix(1);
        while (!sv.empty() && IS_SPACE(sv.back())) sv.remove_suffix(1);
        return sv;
    }

    inline constexpr std::string_view _strip_qualifiers(std::string_view sv) noexcept {
        bool changed;

        do {
            changed = false;
            if (sv.starts_with("const ")) { sv.remove_prefix(6); changed = true; }
            if (sv.starts_with("volatile ")) { sv.remove_prefix(9); changed = true; }
            if (sv.ends_with(" const")) { sv.remove_suffix(6); changed = true; }
            if (sv.ends_with(" volatile")) { sv.remove_suffix(9); changed = true; }
        } while (changed);

        if (sv.starts_with("const volatile ")) sv.remove_prefix(15);
        if (sv.ends_with(" const volatile")) sv.remove_suffix(15);

        return _trim(sv);
    }

    template <typename T>
    constexpr std::string_view _get_type_raw() noexcept {
    #if defined(__clang__)
        constexpr std::string_view PRE = "[T = ";
        constexpr std::string_view SUF = "]";
        constexpr std::string_view FN = __PRETTY_FUNCTION__;
    #elif defined(__GNUC__)
        constexpr std::string_view PRE = "with T = ";
        constexpr std::string_view SUF = ";";
        constexpr std::string_view FN = __PRETTY_FUNCTION__;
    #elif defined(_MSC_VER)
        constexpr std::string_view PRE = "_get_type_raw<";
        constexpr std::string_view SUF = ">(void)";
        constexpr std::string_view FN = __FUNCSIG__;
    #else
        #error "Unsupported compiler"
    #endif

        constexpr auto L = FN.find(PRE);
        static_assert(L != std::string_view::npos, "Failed to parse type name");

        constexpr auto R = FN.find(SUF, L + PRE.size());
        static_assert(R != std::string_view::npos, "Failed to parse type name");

        constexpr size_t START = L + PRE.size();
        return _trim(FN.substr(START, R - START));
    }

    template <typename T>
    constexpr std::string_view _get_type() noexcept {
        return _strip_qualifiers(_get_type_raw<T>());
    }

} // namespace zen
