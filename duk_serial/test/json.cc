//
// Created by rov on 10/4/2025.
//

#include <unordered_map>
#include <duk_serial/json.h>
#include <catch2/catch_test_macros.hpp>

enum class Color {
    NONE,
    RED,
    GREEN,
    BLUE
};

struct Address {
    std::string city;
    int zip{};

    bool operator==(const Address& rhs) const = default;
};

struct Person {
    int age{};
    std::string name;
    Address address;
    Color favoriteColor{};

    bool operator==(const Person&) const = default;
};

struct FriendGroup {
    std::vector<Person> friends;

    bool operator==(const FriendGroup& other) const = default;
};

// define types
namespace duk::type {

template<>
struct Type<Address> : Class<Address,
    Member<"city", &Address::city>,
    Member<"zip", &Address::zip>> {
};

template<>
struct Type<Person> : Class<Person,
    Member<"age", &Person::age>,
    Member<"name", &Person::name>,
    Member<"address", &Person::address>,
    Member<"favoriteColor", &Person::favoriteColor>> {
};

template<>
struct Type<FriendGroup> : Class<FriendGroup,
    Member<"friends", &FriendGroup::friends>> {
};

template<>
struct Type<Color> : Enum<Color,
    Value<"NONE", Color::NONE>,
    Value<"RED", Color::RED>,
    Value<"GREEN", Color::GREEN>,
    Value<"BLUE", Color::BLUE>> {
};

}

TEST_CASE("Basic json serialization", "[json]") {

    SECTION("Basic type roundtrip") {
        const auto input = Address{"New York", 10001};
        const auto json = duk::serial::json_write(input);
        INFO("Json: " << json);
        const auto output = duk::serial::json_read<Address>(json);
        CHECK(input == output);
    }

    SECTION("Basic enum roundtrip") {
        const auto input = Color::RED;
        const auto json = duk::serial::json_write(input);
        INFO("Json: " << json);
        const auto output = duk::serial::json_read<Color>(json);
        CHECK(input == output);
    }

    SECTION("Nested type roundtrip") {
        const auto input = Person{30, "Alice", {"Los Angeles", 90001}, Color::GREEN};
        const auto json = duk::serial::json_write(input);
        INFO("Json: " << json);
        const auto output = duk::serial::json_read<Person>(json);
        CHECK(input == output);
    }

    SECTION("Type with array of structs") {
        const auto input = FriendGroup{
            {
                {25, "Bob", {"Chicago", 60601}, Color::BLUE},
                {28, "Charlie", {"Houston", 77001}, Color::RED}
            }
        };
        const auto json = duk::serial::json_write(input);
        INFO("Json: " << json);
        const auto output = duk::serial::json_read<FriendGroup>(json);
        CHECK(input == output);
    }

    SECTION("Root array") {
        auto input = std::vector<Person>{
            {22, "Dave", {"Phoenix", 85001}, Color::NONE},
            {35, "Eve", {"Philadelphia", 19019}, Color::GREEN}
        };
        const auto json = duk::serial::json_write(input);
        INFO("Json: " << json);
        const auto output = duk::serial::json_read<std::vector<Person>>(json);
        CHECK(input == output);
    }

    SECTION("Array of arrays") {
        auto input = std::vector<std::vector<int>>{
            {1, 2, 3},
            {4, 5, 6}
        };
        const auto json = duk::serial::json_write(input);
        INFO("Json: " << json);
        const auto output = duk::serial::json_read<std::vector<std::vector<int>>>(json);
        CHECK(input == output);
    }

    SECTION("std::set support") {
        auto input = std::set<std::string>{"apple", "banana", "cherry"};
        const auto json = duk::serial::json_write(input);
        INFO("Json: " << json);
        const auto output = duk::serial::json_read<std::set<std::string>>(json);
        CHECK(input == output);
    }

    SECTION("std::unordered_map support") {
        auto input = std::unordered_map<std::string, int>{
            {"one", 1},
            {"two", 2},
            {"three", 3}
        };
        const auto json = duk::serial::json_write(input);
        INFO("Json: " << json);
        const auto output = duk::serial::json_read<std::unordered_map<std::string, int>>(json);
        CHECK(input == output);
    }

    SECTION("std::variant support") {
        auto input = std::variant<int, std::string, Color>{Color::BLUE};
        const auto json = duk::serial::json_write(input);
        INFO("Json: " << json);
        const auto output = duk::serial::json_read<std::variant<int, std::string, Color>>(json);
        CHECK(input == output);
    }

}