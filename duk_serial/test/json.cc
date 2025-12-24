//
// Created by rov on 10/4/2025.
//

#include <duk_serial/json.h>
#include <catch2/catch_test_macros.hpp>


struct Address {
    std::string city;
    int zip{};

    bool operator==(const Address& rhs) const = default;
};

struct Person {
    int age{};
    std::string name;
    Address address;

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
    Member<"address", &Person::address>> {
};

template<>
struct Type<FriendGroup> : Class<FriendGroup,
    Member<"friends", &FriendGroup::friends>> {
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

    SECTION("Nested type roundtrip") {
        const auto input = Person{30, "Alice", {"Los Angeles", 90001}};
        const auto json = duk::serial::json_write(input);
        INFO("Json: " << json);
        const auto output = duk::serial::json_read<Person>(json);
        CHECK(input == output);
    }

    SECTION("Type with array of structs") {
        const auto input = FriendGroup{
            {
                {25, "Bob", {"Chicago", 60601}},
                {28, "Charlie", {"Houston", 77001}}
            }
        };
        const auto json = duk::serial::json_write(input);
        INFO("Json: " << json);
        const auto output = duk::serial::json_read<FriendGroup>(json);
        CHECK(input == output);
    }

    SECTION("Root array") {
        auto input = std::vector<Person>{
            {22, "Dave", {"Phoenix", 85001}},
            {35, "Eve", {"Philadelphia", 19019}}
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

}