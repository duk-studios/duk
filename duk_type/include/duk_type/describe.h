//
// Created by rov on 9/30/2025.
//

#ifndef DUK_TYPE_TYPE_H
#define DUK_TYPE_TYPE_H

#include <string>
#include <memory>

namespace duk::type {

std::string name_of(const std::type_info& info);

template<typename T>
const std::string& name_of() {
    static const std::string typeName = name_of(typeid(T));
    return typeName;
}

template<typename T>
struct Type {
    static constexpr const std::string& name() {
        return name_of<T>();
    }
};

template<typename T>
consteval Type<T> describe() {
    return Type<T>();
}

template<>
struct Type<std::string> {
    static constexpr std::string name() {
        return "std::string";
    }
};

}// namespace duk::type

#endif//DUK_TYPE_TYPE_H