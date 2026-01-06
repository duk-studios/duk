//
// Created by rov on 04/01/2026.
//

#ifndef DUK_SERIAL_DESCRIBE_ENUM_H
#define DUK_SERIAL_DESCRIBE_ENUM_H

#include <stdexcept>
#include <duk_type/describe.h>
#include <duk_type/optional_const.h>
#include <duk_type/string_literal.h>

namespace duk::type {

template<string_literal Name, auto TValue>
struct Value {
    using value_type = decltype(TValue);

    static constexpr value_type value();

    static constexpr std::string_view name();
};

template<typename T, typename... Values>
struct Enum {
    static_assert(std::is_enum_v<T>, "T must be an enum type");
    static_assert((std::is_same_v<T, typename Values::value_type> && ...), "All Values must belong to enum T");

    static constexpr const std::string& name();

    static constexpr T value_of(std::string_view name);

    static constexpr std::string_view name_of(T value);

    static constexpr uint32_t value_count();
};

template<typename, typename = void>
struct is_enum_description_type : std::false_type {};

template<typename T>
struct is_enum_description_type<T, std::void_t<decltype(T::value_count())>> : std::true_type {};

template<typename T>
consteval bool is_enum_description() {
    return is_enum_description_type<std::decay_t<T>>::value;
}

template<typename T>
consteval bool is_enum() {
    return is_enum_description<decltype(type::describe<T>())>();
}

template<string_literal Name, auto TValue>
constexpr Value<Name, TValue>::value_type Value<Name, TValue>::value() {
    return TValue;
}

template<string_literal Name, auto TValue>
constexpr std::string_view Value<Name, TValue>::name() {
    return Name.value;
}

template<typename T, typename... Values>
constexpr const std::string& Enum<T, Values...>::name() {
    return type::name_of<T>();
}

template<typename T, typename... Values>
constexpr T Enum<T, Values...>::value_of(std::string_view name) {
    constexpr auto names = std::array{Values::name()...};
    constexpr auto values = std::array{Values::value()...};
    for (size_t i = 0; i < names.size(); ++i) {
        if (names[i] == name) {
            return values[i];
        }
    }
    throw std::invalid_argument("Invalid enum name");
}

template<typename T, typename... Values>
constexpr std::string_view Enum<T, Values...>::name_of(T value) {
    constexpr auto names = std::array{Values::name()...};
    constexpr auto values = std::array{Values::value()...};
    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i] == value) {
            return names[i];
        }
    }
    throw std::invalid_argument("Invalid enum value");
}

template<typename T, typename... Values>
constexpr uint32_t Enum<T, Values...>::value_count() {
    return sizeof...(Values);
}

}// namespace duk::type

#endif//DUK_SERIAL_DESCRIBE_ENUM_H