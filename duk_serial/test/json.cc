//
// Created by rov on 10/4/2025.
//

#include <duk_serial/serial.h>
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

struct Transform {
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    bool operator==(const Transform& other) const {
        return position == other.position && scale == other.scale && glm::all(glm::epsilonEqual(rotation, other.rotation, glm::epsilon<float>()));
    }
};

namespace duk::serial {

template<>
void define_class_parser(ClassParser<Address>& parser) {
    parser.add("city", &Address::city);
    parser.add("zip", &Address::zip);
}

template<>
void define_class_parser(ClassParser<Person>& parser) {
    parser.add("age", &Person::age);
    parser.add("name", &Person::name);
    parser.add("address", &Person::address);
}

template<>
void define_class_parser(ClassParser<FriendGroup>& parser) {
    parser.add("friends", &FriendGroup::friends);
}

template<>
void define_class_parser(ClassParser<Transform>& parser) {
    parser.add("position", &Transform::position);
    parser.add("rotation", &Transform::rotation);
    parser.add("scale", &Transform::scale);
}

}

TEST_CASE("JsonParser basic usage", "[parser]") {
    using namespace duk::serial;
    struct Foo {
        int a;
        std::string b;
    };
    ClassParser<Foo> fooParser;

    fooParser.add("a", &Foo::a);
    fooParser.add("b", &Foo::b);

    SECTION("Write object to JSON") {
        Foo p{42, "abcde"};

        rapidjson::Document doc;
        doc.SetObject();
        rapidjson::Value json(rapidjson::kObjectType);
        fooParser.write(doc, json, p);

        REQUIRE(json.HasMember("a"));
        REQUIRE(json["a"].IsInt());
        REQUIRE(json["a"].GetInt() == 42);

        REQUIRE(json.HasMember("b"));
        REQUIRE(json["b"].IsString());
        REQUIRE(json["b"].GetString() == std::string("abcde"));
    }

    SECTION("Read object from JSON") {
        const char* jsonStr = R"({"a": 13, "b": "xyz"})";
        rapidjson::Document doc;
        doc.Parse(jsonStr);

        REQUIRE_FALSE(doc.HasParseError());
        REQUIRE(doc.IsObject());

        Foo p;
        fooParser.read(doc, p);

        REQUIRE(p.a == 13);
        REQUIRE(p.b == "xyz");
    }

    SECTION("Round-trip serialization") {
        Foo original{42, "abcde"};

        // Write to JSON
        rapidjson::Document doc;
        doc.SetObject();
        rapidjson::Value json(rapidjson::kObjectType);
        fooParser.write(doc, json, original);

        // Convert JSON value to string (optional, for visual debugging)
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        json.Accept(writer);
        std::string jsonStr = buffer.GetString();
        INFO("Serialized JSON: " << jsonStr);

        // Parse back from JSON string
        rapidjson::Document parsedDoc;
        parsedDoc.Parse(jsonStr.c_str());
        REQUIRE_FALSE(parsedDoc.HasParseError());

        Foo roundTripped;
        fooParser.read(parsedDoc, roundTripped);

        // Check that the original and round-tripped objects match
        REQUIRE(roundTripped.a == original.a);
        REQUIRE(roundTripped.b == original.b);
    }

    SECTION("Convenience methods with specialized parsers") {
        const auto original = Address{"fooland", 11110};
        const auto json = write_json(original);
        INFO("JSON: " << json);
        const auto roundTripped = read_json<Address>(json);
        REQUIRE(original == roundTripped);
    }

    SECTION("Nested type") {
        const auto original = Person{33, "Dana", {"New York", 10001}};
        const auto json = write_json(original);
        INFO("JSON: " << json);
        const auto roundTripped = read_json<Person>(json);
        REQUIRE(roundTripped == original);
    }

    SECTION("Type with array") {
        const auto original = FriendGroup{
            {
                {25, "Tob", {"Fooland", 10021}},
                {27, "Bob", {"Booland", 10031}},
                {29, "Rob", {"Mooland", 10051}}
            }
        };
        const auto json = write_json(original);
        INFO("JSON: " << json);
        const auto roundTripped = read_json<FriendGroup>(json);
        REQUIRE(roundTripped == original);
    }

    SECTION("Root array") {
        const auto original = std::vector<Person>{
        {25, "Tob", {"Fooland", 10021}},
        {27, "Bob", {"Booland", 10031}},
        {29, "Rob", {"Mooland", 10051}}
        };
        const auto json = write_json(original);
        INFO("JSON: " << json);
        const auto roundTripped = read_json<std::vector<Person>>(json);
        REQUIRE(roundTripped == original);
    }

    SECTION("custom parser support (glm)") {
        const auto original = Transform{
            {140, 40.0, 20.0f},
            glm::vec3{30.0f, 0.0f, 180.0f},
            {1.0f, 0.5f, 2.0f}
        };
        const auto json = write_json(original);
        INFO("JSON: " << json);
        const auto roundTripped = read_json<Transform>(json);
        REQUIRE(original == roundTripped);
    }
}