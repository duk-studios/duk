//
// Created by rov on 10/4/2025.
//

#include <duk_type/describe.h>
#include <duk_type/describe_class.h>
#include <duk_type/describe_container.h>
#include <duk_type/describe_enum.h>

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

struct Complex {
    std::vector<Bar> bars;
    std::array<int, 5> numbers;
    std::set<std::string> names;
};

enum class DummyEnum {
    VALUE_ONE,
    VALUE_TWO,
    VALUE_THREE
};

}

namespace duk::type {

using namespace foo;
// clang-format off
template<>
struct Type<Bar> : Class<Bar,
    Member<"a", &Bar::a>,
    Member<"b", &Bar::b>> {
};

template<>
struct Type<Baz> : Class<Baz,
    Member<"bar", &Baz::bar>,
    Member<"c", &Baz::c>> {
};

template<>
struct Type<Complex> : Class<Complex,
    Member<"bars", &Complex::bars>,
    Member<"numbers", &Complex::numbers>,
    Member<"names", &Complex::names>> {

};

template<>
struct Type<DummyEnum> : Enum<DummyEnum,
    Value<"VALUE_ONE", DummyEnum::VALUE_ONE>,
    Value<"VALUE_TWO", DummyEnum::VALUE_TWO>,
    Value<"VALUE_THREE", DummyEnum::VALUE_THREE>> {
};

// clang-format on
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
        description.visit_members([&](auto member) {
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
        auto visitor = [&](this const auto& self, auto member) {
            constexpr auto memberDescription = member.describe();
            visitedMembers[member.name()] = memberDescription.name();
            if constexpr (duk::type::is_class_description<decltype(memberDescription)>()) {
                memberDescription.visit_members(self, member.value());
            }
        };
        Baz baz{{42, 3.14f}, 2.718};
        auto description = duk::type::describe<Baz>();
        description.visit_members(visitor, baz);

        CHECK(visitedMembers.size() == 4);
        CHECK(visitedMembers["bar"] == "foo::Bar");
        CHECK(visitedMembers["a"] == "int");
        CHECK(visitedMembers["b"] == "float");
        CHECK(visitedMembers["c"] == "double");
    }

    SECTION("We inspect basic container types") {
        std::vector<int> vec = {1, 2, 3};
        auto vecDescription = duk::type::describe<decltype(vec)>();
        CHECK(vecDescription.name() == "std::vector<int>");
        CHECK(vecDescription.size(vec) == 3);
        CHECK(duk::type::is_container_description<decltype(vecDescription)>());

        std::array<float, 4> arr = {0.1f, 0.2f, 0.3f, 0.4f};
        auto arrDescription = duk::type::describe<decltype(arr)>();
        CHECK(arrDescription.name() == "std::array<float,4>");
        CHECK(arrDescription.size(arr) == 4);
        CHECK(duk::type::is_container_description<decltype(arrDescription)>());

        std::set<std::string> strSet = {"foo", "bar", "baz"};
        auto setDescription = duk::type::describe<decltype(strSet)>();
        CHECK(setDescription.name() == "std::set<std::string>");
        CHECK(setDescription.size(strSet) == 3);
        CHECK(duk::type::is_container_description<decltype(setDescription)>());
    }

    SECTION("We can iterate over container elements") {
        std::vector<std::string> vec = {"one", "two", "three"};
        auto vecDescription = duk::type::describe<decltype(vec)>();

        std::vector<std::string> visitedElements;
        vecDescription.visit_elements([&](const auto& element) {
            visitedElements.push_back(element);
        }, vec);

        CHECK(visitedElements.size() == 3);
        CHECK(visitedElements[0] == "one");
        CHECK(visitedElements[1] == "two");
        CHECK(visitedElements[2] == "three");
    }

    SECTION("We can describe enums and query values and their names") {
        using namespace foo;
        constexpr auto enumDescription = duk::type::describe<DummyEnum>();
        CHECK(enumDescription.name() == "foo::DummyEnum");
        CHECK(enumDescription.value_count() == 3);
        CHECK(enumDescription.name_of(DummyEnum::VALUE_ONE) == "VALUE_ONE");
        CHECK(enumDescription.name_of(DummyEnum::VALUE_TWO) == "VALUE_TWO");
        CHECK(enumDescription.name_of(DummyEnum::VALUE_THREE) == "VALUE_THREE");
        CHECK(enumDescription.value_of("VALUE_ONE") == DummyEnum::VALUE_ONE);
        CHECK(enumDescription.value_of("VALUE_TWO") == DummyEnum::VALUE_TWO);
        CHECK(enumDescription.value_of("VALUE_THREE") == DummyEnum::VALUE_THREE);
        CHECK_THROWS(enumDescription.value_of("unknown"));
        CHECK_THROWS(enumDescription.name_of(static_cast<DummyEnum>(999)));

        constexpr auto constexprValue = enumDescription.value_of("VALUE_THREE");
        static_assert(constexprValue == DummyEnum::VALUE_THREE);

        volatile auto valueName = "VALUE_TWO";
        volatile auto runtimeValue = enumDescription.value_of(valueName);
        CHECK(runtimeValue == DummyEnum::VALUE_TWO);
    }
}