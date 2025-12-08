//
// Created by rov on 10/4/2025.
//

#include <duk_type/type.h>

#include <catch2/catch_test_macros.hpp>

#include <unordered_map>

namespace foo {

struct Bar {
    int a;
    float b;
};

struct Baz {
    Bar bar;
    double c;
};

}

namespace duk::type {

using namespace foo;

template<>
consteval auto describe<Bar>() {
    return ClassVisitor<Bar,
        Member<"a", &Bar::a>,
        Member<"b", &Bar::b>
    >();
}

template<>
consteval auto describe<Baz>() {
    return ClassVisitor<Baz,
        Member<"bar", &Baz::bar>,
        Member<"c", &Baz::c>
    >();
}

}

TEST_CASE("Basic type information can be retrieved", "[type]") {

    SECTION("We can describe a type and its members") {
        using namespace foo;
        auto description = duk::type::describe<Bar>();
        CHECK(description.name() == "foo::Bar");
        CHECK(description.member_count() == 2);
    }

    SECTION("We can visit members of a type and inspect its members' types") {
        using namespace foo;

        Bar bar{42, 3.14f};

        auto description = duk::type::describe<Bar>();
        std::unordered_map<std::string_view, std::string_view> visitedMembers;
        description.visit([&](auto member) {
            constexpr auto memberDescription = member.describe();
            visitedMembers[member.name()] = memberDescription.name();
        }, bar);

        CHECK(visitedMembers.size() == 2);
        CHECK(visitedMembers["a"] == "int");
        CHECK(visitedMembers["b"] == "float");
    }

    SECTION("We can have nested types with members") {
        using namespace foo;


        std::unordered_map<std::string_view, std::string_view> visitedMembers;
        auto visitor = [&](this const auto &self, auto member) {
            constexpr auto memberDescription = member.describe();
            visitedMembers[member.name()] = memberDescription.name();
            memberDescription.visit(self, member.value());
        };
        Baz baz{{42, 3.14f}, 2.718};
        auto description = duk::type::describe<Baz>();
        description.visit(visitor, baz);

        CHECK(visitedMembers.size() == 4);
        CHECK(visitedMembers["bar"] == "foo::Bar");
        CHECK(visitedMembers["a"] == "int");
        CHECK(visitedMembers["b"] == "float");
        CHECK(visitedMembers["c"] == "double");
    }
}