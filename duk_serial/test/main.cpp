//
// Created by Ricardo on 24/02/2024.
//

#include <duk_log/log.h>
#include <duk_serial/json_serializer.h>
#include <string>
#include <vector>

struct Foo {
    uint32_t a;
    std::string b;
};

struct Boo {
    uint32_t var1;
    Foo foo;
    float var2;
};

struct Poo {
    std::vector<Boo> boos;
    float a;
};

namespace duk::serial {

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, Foo& foo) {
    visitor->visit_member(foo.a, MemberDescription("a"));
    visitor->visit_member(foo.b, MemberDescription("b"));
}

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, Boo& boo) {
    visitor->visit_member(boo.var1, MemberDescription("var1"));
    visitor->visit_member_object(boo.foo, MemberDescription("foo"));
    visitor->visit_member(boo.var2, MemberDescription("var2"));
}

template<>
void read_array<std::vector<Boo>>(JsonReader* reader, std::vector<Boo>& vector, size_t size) {
    vector.reserve(size);
    for (size_t i = 0; i < size; i++) {
        reader->visit_member_object(vector.emplace_back(), MemberDescription(i));
    }
}

template<>
void write_array<std::vector<Boo>>(JsonWriter* writer, std::vector<Boo>& vector) {
    for (size_t i = 0; i < vector.size(); i++) {
        writer->visit_member_object(vector[i], MemberDescription(i));
    }
}

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, Poo& poo) {
    visitor->visit_member_array(poo.boos, MemberDescription("boos"));
    visitor->visit_member(poo.a, MemberDescription("a"));
}

}// namespace duk::serial

const char* booJsonStr = R"(
{
   "var1": 20,
   "foo": {
      "a": 1,
      "b": "safe"
   },
   "var2": 32.54
}
)";

const char* pooJsonStr = R"(
{
    "boos": [
        {
            "var1": 20,
            "foo": {
                "a": 1,
                "b": "safe"
            },
            "var2": 32.54
        },
        {
            "var1": 50,
            "foo": {
                "a": 3645,
                "b": "safe 2"
            },
            "var2": -423.5
        }
    ],
    "a": 1000.0
}
)";

int main() {
    duk::serial::JsonReader jsonReader(pooJsonStr);

    Poo poo = {};
    jsonReader.visit(poo);

    duk::serial::JsonWriter jsonWriter;
    jsonWriter.visit(poo);

    duk::log::debug("{}", jsonWriter.pretty_print()).wait();

    return 0;
}