#pragma once

#include <iostream>

namespace zen {

    static inline const std::string_view TAB = "    ";

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
    inline void _print_type() noexcept
    {
        constexpr std::string_view FN = __PRETTY_FUNCTION__;
        constexpr size_t L = FN.find("[with T = ") + sizeof("[with T = ") - 1;
        constexpr size_t R = FN.rfind("]");
        static_assert(L < R);

        std::cout << FN.substr(L, (R - L));
    }

    template <typename T>
    void debug(const T& data, std::string_view suffix = {}) noexcept
    {
        dbg_trait<T>::debug(data);
        std::cout << suffix;
    }

    template <typename T>
    void debug(std::string_view prefix, const T& data, std::string_view suffix = {}) noexcept
    {
        std::cout << prefix;
        dbg_trait<T>::debug(data);
        std::cout << suffix;
    }

    template <typename T>
    void pdebug(const T& data, std::string_view suffix = {}, int indent = 0) noexcept
    {
        dbg_trait<T>::pdebug(data, indent);
        std::cout << suffix;
    }

    template <typename T>
    void pdebug(std::string_view prefix, const T& data, std::string_view suffix = {}, int indent = 0) noexcept
    {
        std::cout << prefix;
        dbg_trait<T>::pdebug(data, indent);
        std::cout << suffix;
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
            zen::pdebug(*it, {}, indent + 1);
            std::cout << ",\n";
        }

        _print_tab(indent);
        std::cout << ']';
    }

    template <typename T>
    inline void _print_ptr(const T* const data_ptr) noexcept
    {
        std::cout << "<"; _print_type<T>();
        std::cout<< " *> ";

        if (data_ptr) zen::debug(*data_ptr);
        else std::cout << "(nullptr)";
    }

    template <typename T>
    inline void _pprint_ptr(const T* const data_ptr, int indent = 0) noexcept
    {
        std::cout << "<"; _print_type<T>();
        std::cout<< " *> ";

        if (data_ptr) zen::pdebug(*data_ptr, {}, indent);
        else std::cout << "(nullptr)";
    }

} // namespace zen

#define ZEN_VAR(var)   do { ::zen::debug ("\n" #var " = ", (var)); } while(0)
#define ZEN_VAR_P(var) do { ::zen::pdebug("\n" #var " = ", (var)); } while(0)

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
    static void debug(const std::array<T, N>& data) noexcept { zen::_print_type(data.begin(), data.end()); }

    static void pdebug(const std::array<T, N>& data, int indent = 0) noexcept
    {
        zen::_pprint_it(data.begin(), data.end(), indent);
    }
};

#include <span>

template <typename T, size_t N>
struct zen::dbg_trait<std::span<T, N>> {
    static void debug(const std::span<T, N>& data) noexcept { zen::_print_type(data.begin(), data.end()); }

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
            zen::_print_tab(indent + 1);
            zen::dbg_trait<std::pair<K, V>>::pdebug(ITEM, indent + 1);
            std::cout << ",\n";
        }
        zen::_print_tab(indent);
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

#include <memory>

template <typename T>
struct zen::dbg_trait<std::unique_ptr<T>> {
    static void debug(const std::unique_ptr<T>& data) noexcept { zen::_print_ptr(data.get()); }

    static void pdebug(const std::unique_ptr<T>& data, int indent = 0) noexcept
    {
        zen::_pprint_ptr(data.get(), indent);
    }
};

template <typename T>
struct zen::dbg_trait<std::shared_ptr<T>> {
    static void debug(const std::shared_ptr<T>& data) noexcept { zen::_print_ptr(data.get()); }

    static void pdebug(const std::shared_ptr<T>& data, int indent = 0) noexcept
    {
        zen::_pprint_ptr(data.get(), indent);
    }
};

template <typename T>
struct zen::dbg_trait<std::weak_ptr<T>> {
    static void debug(const std::weak_ptr<T>& data) noexcept { zen::_print_ptr(data.lock().get()); }

    static void pdebug(const std::weak_ptr<T>& data, int indent = 0) noexcept
    {
        zen::_pprint_ptr(data.lock().get(), indent);
    }
};

#include <optional>

template <typename T>
struct zen::dbg_trait<std::optional<T>> {
    static void debug(const std::optional<T>& data) noexcept
    {
        if (data.has_value())
            zen::dbg_trait<T>::debug(data.value());
        else
            std::cout << "optional<" << typeid(T).name() << ">(nullopt)";
    }

    static void pdebug(const std::optional<T>& data, int indent = 0) noexcept
    {
        if (data.has_value())
            zen::dbg_trait<T>::pdebug(data.value(), indent);
        else
            std::cout << "optional<" << typeid(T).name() << ">(nullopt)";
    }
};

#include <variant>

template <typename... Types>
struct zen::dbg_trait<std::variant<Types...>> {
    static void debug(const std::variant<Types...>& data) noexcept
    {
        std::visit(
            [](const auto& value) { zen::dbg_trait<std::decay_t<decltype(value)>>::debug(value); },
            data
        );
    }

    static void pdebug(const std::variant<Types...>& data, int indent = 0) noexcept
    {
        std::visit(
            [&](const auto& value) { zen::dbg_trait<std::decay_t<decltype(value)>>::pdebug(value, indent); },
            data
        );
    }
};
