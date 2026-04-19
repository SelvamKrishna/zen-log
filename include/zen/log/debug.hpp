#pragma once

#include <iostream>

namespace zen {

    static inline const char* TAB = "    ";

    template <typename T>
    struct dbg_trait {
        static void debug(const T& data) noexcept { std::cout << data; }
        static void pdebug(const T& data, int indent = 0) noexcept
        {
            (void)indent;
            dbg_trait<T>::debug(data);
        }
    };

    template <typename T>
    void debug(const T& data) noexcept { dbg_trait<T>::debug(data); }

    template <typename T>
    void debug(std::string_view prefix, const T& data) noexcept
    {
        std::cout << prefix << " = ";
        dbg_trait<T>::debug(data);
    }

    template <typename T>
    void pdebug(const T& data, int indent = 0) noexcept { dbg_trait<T>::pdebug(data, indent); }

    template <typename T>
    void pdebug(std::string_view prefix, const T& data, int indent = 0) noexcept
    {
        std::cout << prefix << " = ";
        dbg_trait<T>::pdebug(data, indent);
    }

    inline void _print_tab(int level) noexcept { for (int i = 0; i < level; ++i) std::cout << TAB; }

    template <std::forward_iterator It>
    inline void _print_it(It begin, It end) noexcept
    {
        std::cout << '[';
        for (It it = begin; it != end; ++it)
        {
            zen::debug(*it);
            if (std::next(it) != end) std::cout << ", ";
        }
        std::cout << ']';
    }

    template <std::forward_iterator It>
    inline void _pprint_it(It begin, It end, int indent = 0) noexcept
    {
        std::cout << "[\n";
        for (It it = begin; it != end; ++it)
        {
            _print_tab(indent + 1);
            zen::pdebug(*it, indent + 1);
            std::cout << ",\n";
        }
        _print_tab(indent);
        std::cout << ']';
    }

} // namespace zen

#define ZEN_VAR(var)   do { ::zen::debug("\n" #var, var); } while(0)
#define ZEN_VAR_P(var) do { ::zen::pdebug("\n" #var, var); } while(0)

template <typename K, typename V>
struct zen::dbg_trait<std::pair<K, V>> {
    static void debug(const std::pair<K, V>& data) noexcept
    {
        zen::dbg_trait<K>::debug(data.first);
        std::cout << ": ";
        zen::dbg_trait<V>::debug(data.second);
    }

    static void pdebug(const std::pair<K, V>& data, int indent = 0) noexcept
    {
        zen::dbg_trait<K>::pdebug(data.first, indent);
        std::cout << ": ";
        zen::dbg_trait<V>::pdebug(data.second, indent);
    }
};

#include <array>

template <typename T, size_t N>
struct zen::dbg_trait<std::array<T, N>> {
    static void debug(const std::array<T, N>& data) noexcept { zen::_print_it(data.begin(), data.end()); }

    static void pdebug(const std::array<T, N>& data, int indent = 0) noexcept
    {
        zen::_pprint_it(data.begin(), data.end(), indent);
    }
};

#include <span>

template <typename T, size_t N>
struct zen::dbg_trait<std::span<T, N>> {
    static void debug(const std::span<T, N>& data) noexcept { zen::_print_it(data.begin(), data.end()); }

    static void pdebug(const std::span<T, N>& data, int indent = 0) noexcept
    {
        zen::_pprint_it(data.begin(), data.end(), indent);
    }
};

#include <unordered_map>

template <typename K, typename V>
struct zen::dbg_trait<std::unordered_map<K, V>> {
    static void debug(const std::unordered_map<K, V>& data) noexcept
    {
        std::cout << '{';
        bool first = true;
        for (const std::pair<const K, const V>& ITEM : data)
        {
            if (!first) std::cout << ", ";
            zen::dbg_trait<std::pair<K, V>>::debug(ITEM);
            first = false;
        }
        std::cout << '}';
    }

    static void pdebug(const std::unordered_map<K, V>& data, int indent = 0) noexcept
    {
        std::cout << "{\n";
        for (const std::pair<const K, const V>& ITEM : data)
        {
            _print_tab(indent + 1);
            zen::dbg_trait<std::pair<K, V>>::pdebug(ITEM, indent + 1);
            std::cout << ",\n";
        }
        _print_tab(indent);
        std::cout << '}';
    }
};

#include <vector>

template <typename T>
struct zen::dbg_trait<std::vector<T>> {
    static void debug(const std::vector<T>& data) noexcept { zen::_print_it(data.begin(), data.end()); }

    static void pdebug(const std::vector<T>& data, int indent = 0) noexcept
    {
        zen::_pprint_it(data.begin(), data.end(), indent);
    }
};
