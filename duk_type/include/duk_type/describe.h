//
// Created by rov on 9/30/2025.
//

#ifndef DUK_TYPE_TYPE_H
#define DUK_TYPE_TYPE_H

#include <duk_type/string_literal.h>

#include <string>
#include <memory>

namespace duk::type {

std::string name_of(const std::type_info& info);

template<typename T>
std::string_view name_of() {
    static const std::string typeName = name_of(typeid(T));
    return typeName;
}

template<typename T>
class Type {
public:
    static constexpr std::string_view name() {
        return name_of<T>();
    }

    template<typename Visitor>
    static constexpr void visit_members(Visitor&&, T&) {
        // Default implementation does nothing
    }
};

template<typename T>
consteval auto describe() {
    return Type<T>();
}

template<string_literal Name, auto MemberPointer>
class Member {
private:
    template<typename>
    struct member_pointer_traits {
        static_assert(false, "member_pointer_traits must be specialized for member pointers");
    };

    template<typename Class, typename T>
    struct member_pointer_traits<T Class::*> {
        using type = T;
        using class_type = Class;
    };
public:
    using MemberPointerType = member_pointer_traits<decltype(MemberPointer)>;
    using MemberType = typename MemberPointerType::type;
    using ClassType = typename MemberPointerType::class_type;

    static consteval auto describe() {
        return type::describe<MemberType>();
    }

    static constexpr MemberType ClassType::* pointer() {
        return MemberPointer;
    }

    static constexpr std::string_view name() {
        return Name.value;
    }

    explicit Member(ClassType& instance)
        : m_instance(instance) {
    }

    MemberType& value() {
        return m_instance.*pointer();
    }

    const MemberType& value() const {
        return m_instance.*pointer();
    }

protected:
    ClassType& m_instance;
};

template<typename T, typename ...Members>
class Class  {
public:
    static_assert((std::is_same_v<T, typename Members::ClassType> && ...), "All Members must belong to the same Class");

    static constexpr std::string_view name() {
        return name_of<T>();
    }

    template<typename Visitor>
    static constexpr void visit_members(Visitor&& visitor, T& instance) {
        (std::invoke(std::forward<Visitor>(visitor), Members(instance)), ...);
    }

    static constexpr uint32_t member_count() {
        return sizeof...(Members);
    }
};

}

#endif //DUK_TYPE_TYPE_H