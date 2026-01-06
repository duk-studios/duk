//
// Created by rov on 22/12/2025.
//

#ifndef DUK_DESCRIBE_CONTAINER_H
#define DUK_DESCRIBE_CONTAINER_H

#include <duk_type/describe.h>

#include <fmt/format.h>

#include <iterator>
#include <vector>
#include <array>
#include <set>

namespace duk::type {

template<typename T>
struct Container {
    using container_type = T;
    using value_type = typename T::value_type;

    static constexpr const std::string& name() {
        return name_of<T>();
    }

    static constexpr auto begin(T& container) {
        return std::begin(container);
    }

    static constexpr auto begin(const T& container) {
        return std::cbegin(container);
    }

    static constexpr auto end(T& container) {
        return std::end(container);
    }

    static constexpr auto end(const T& container) {
        return std::cend(container);
    }

    static constexpr auto size(const T& container) {
        return std::size(container);
    }

    template<typename Visitor, typename... Args>
    static constexpr void visit_elements(Visitor&& visitor, T& container, const Args&... args) {
        for (auto& element: container) {
            std::invoke(std::forward<Visitor>(visitor), element, args...);
        }
    }

    template<typename Visitor, typename... Args>
    static constexpr void visit_elements(Visitor&& visitor, const T& container, const Args&... args) {
        for (auto& element: container) {
            std::invoke(std::forward<Visitor>(visitor), element, args...);
        }
    }
};

template<typename T>
struct Type<std::vector<T>> : Container<std::vector<T>> {
    static constexpr const std::string& name() {
        static std::string typeName = [] {
            constexpr auto valueDescription = describe<T>();
            return fmt::format("std::vector<{}>", valueDescription.name());
        }();
        return typeName;
    }

    static constexpr auto insert_back(std::vector<T>& container, const T& value) {
        return container.insert(container.end(), value);
    }

    static constexpr auto insert_back(std::vector<T>& container, T&& value) {
        return container.insert(container.end(), std::move(value));
    }

    static constexpr T& at(std::vector<T>& container, size_t index) {
        return container.at(index);
    }

    static constexpr const T& at(const std::vector<T>& container, size_t index) {
        return container.at(index);
    }
};

template<typename T, size_t N>
struct Type<std::array<T, N>> : Container<std::array<T, N>> {
    static constexpr T& at(std::array<T, N>& container, size_t index) {
        return container.at(index);
    }

    static constexpr const T& at(const std::array<T, N>& container, size_t index) {
        return container.at(index);
    }
};

template<typename T>
struct Type<std::set<T>> : Container<std::set<T>> {
    static constexpr const std::string& name() {
        static std::string typeName = [] {
            constexpr auto valueDescription = describe<T>();
            return fmt::format("std::set<{}>", valueDescription.name());
        }();
        return typeName;
    }

    static constexpr auto insert_back(std::set<T>& container, const T& value) {
        return container.insert(container.end(), value);
    }

    static constexpr auto insert_back(std::set<T>& container, T&& value) {
        return container.insert(container.end(), std::move(value));
    }
};

template<typename, typename = void>
struct has_insert_back_method : std::false_type {};

template<typename T>
struct has_insert_back_method<T, std::void_t<decltype(T::insert_back(std::declval<typename T::container_type>(), std::declval<typename T::value_type>()))>> : std::true_type {};

template<typename T>
constexpr bool has_insert_back(const T&) {
    return has_insert_back_method<std::decay_t<T>>::value;
}

template<typename, typename = void>
struct is_container_description_type : std::false_type {};

template<typename T>
struct is_container_description_type<T, std::void_t<decltype(T::size(std::declval<typename T::container_type>()))>> : std::true_type {};

template<typename T>
constexpr bool is_container_description() {
    return is_container_description_type<std::decay_t<T>>::value;
}

template<typename T>
constexpr bool is_container() {
    return is_container_description<decltype(type::describe<T>())>();
}

}// namespace duk::type

#endif//DUK_DESCRIBE_CONTAINER_H