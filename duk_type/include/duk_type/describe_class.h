//
// Created by rov on 22/12/2025.
//

#ifndef DUK_TYPE_DESCRIBE_CLASS_H
#define DUK_TYPE_DESCRIBE_CLASS_H

#include <duk_type/describe.h>
#include <duk_type/optional_const.h>
#include <duk_type/string_literal.h>

namespace duk::type {

template<string_literal Name, auto MemberPointer>
struct Member {
    template<typename>
    struct member_pointer_traits {
        static_assert(false, "member_pointer_traits must be specialized for member pointers");
    };

    template<typename Class, typename T>
    struct member_pointer_traits<T Class::*> {
        using type = T;
        using class_type = Class;
    };

    using member_pointer_type = member_pointer_traits<decltype(MemberPointer)>;
    using member_type = typename member_pointer_type::type;
    using class_type = typename member_pointer_type::class_type;

    static constexpr member_type& get(class_type& self);

    static constexpr const member_type& get(const class_type& self);

    static constexpr std::string_view name();
};

template<typename T, typename ...Members>
struct Class {
    static_assert((std::is_same_v<T, typename Members::class_type> && ...), "All Members must belong to the same Class");

    static constexpr const std::string& name();

    template<typename Visitor, typename ...Args>
    static constexpr void visit_members(Visitor&& visitor, T& instance, const Args& ...args);

    template<typename Visitor, typename ...Args>
    static constexpr void visit_members(Visitor&& visitor, const T& instance, const Args& ...args);

    static constexpr uint32_t member_count();

private:

    template<typename Member, bool IsConst>
    class MemberAccessor {
    public:
        using class_type = optional_const_t<typename Member::class_type, IsConst>;
        using member_type = optional_const_t<typename Member::member_type, IsConst>;

        static consteval auto describe() {
            return type::describe<member_type>();
        }

        static constexpr std::string_view name() {
            return Member::name();
        }

        explicit MemberAccessor(class_type& instance)
            : m_instance(instance) {
        }

        auto&& value() const {
            return Member::get(m_instance);
        }

    protected:
        class_type& m_instance;
    };
};

template<typename, typename = void>
struct is_class_description_type : std::false_type {};

template<typename T>
struct is_class_description_type<T, std::void_t<decltype(T::member_count())>> : std::true_type {};

template<typename T>
consteval bool is_class_description() {
    return is_class_description_type<std::decay_t<T>>::value;
}

template<typename T>
consteval bool is_class() {
    return is_class_description<decltype(type::describe<T>())>();
}

template<string_literal Name, auto MemberPointer>
constexpr Member<Name, MemberPointer>::member_type& Member<Name, MemberPointer>::get(class_type& self) {
    return self.*MemberPointer;
}

template<string_literal Name, auto MemberPointer>
constexpr const Member<Name, MemberPointer>::member_type& Member<Name, MemberPointer>::get(const class_type& self) {
    return self.*MemberPointer;
}

template<string_literal Name, auto MemberPointer>
constexpr std::string_view Member<Name, MemberPointer>::name() {
    return Name.value;
}

template<typename T, typename ... Members>
constexpr const std::string& Class<T, Members...>::name() {
    return name_of<T>();
}

template<typename T, typename ...Members>
template<typename Visitor, typename ...Args>
constexpr void Class<T, Members...>::visit_members(Visitor&& visitor, T& instance, const Args& ...args) {
    (std::invoke(std::forward<Visitor>(visitor), MemberAccessor<Members, false>(instance), args...), ...);
}

template<typename T, typename ...Members>
template<typename Visitor, typename ...Args>
constexpr void Class<T, Members...>::visit_members(Visitor&& visitor, const T& instance, const Args& ...args) {
    (std::invoke(std::forward<Visitor>(visitor), MemberAccessor<Members, true>(instance), args...), ...);
}

template<typename T, typename ... Members>
constexpr uint32_t Class<T, Members...>::member_count() {
    return sizeof...(Members);
}

}

#endif //DUK_TYPE_DESCRIBE_CLASS_H