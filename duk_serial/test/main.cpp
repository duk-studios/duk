//
// Created by Ricardo on 24/02/2024.
//

#include <duk_log/log.h>
#include <duk_serial/json/serializer.h>
#include <string>
#include <vector>

namespace duk::serial {

template<>
inline void from_json<Foo>(const rapidjson::Value& json, Foo& foo) {
    from_json_member(json, "a", foo.a);
    from_json_member(json, "b", foo.b);
}

template<>
inline void to_json<Foo>(rapidjson::Document& document, rapidjson::Value& json, const Foo& foo) {
    to_json_member(document, json, "a", foo.a);
    to_json_member(document, json, "b", foo.b);
}

template<>
inline void from_json<Boo>(const rapidjson::Value& json, Boo& boo) {
    from_json_member(json, "foo", boo.foo);
    from_json_member(json, "var1", boo.var1);
    from_json_member(json, "var2", boo.var2);
}

template<>
inline void to_json<Boo>(rapidjson::Document& document, rapidjson::Value& json, const Boo& boo) {
    to_json_member(document, json, "foo", boo.foo);
    to_json_member(document, json, "var1", boo.var1);
    to_json_member(document, json, "var2", boo.var2);
}

template<>
inline void from_json<Poo>(const rapidjson::Value& json, Poo& poo) {
    from_json_member(json, "boos", poo.boos);
    from_json_member(json, "a", poo.a);
}

template<>
inline void to_json<Poo>(rapidjson::Document& document, rapidjson::Value& json, const Poo& poo) {
    to_json_member(document, json, "boos", poo.boos);
    to_json_member(document, json, "a", poo.a);
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
    // rapidjson::Value jsonValue;
    // std::vector<Foo> foos;
    // duk::serial::from_json<Foo>(jsonValue, foos);

    Poo poo = {};
    duk::serial::read_json(pooJsonStr, poo);

    std::ostringstream oss;
    duk::serial::write_json(oss, poo, true);

    duk::log::debug("{}", oss.str()).wait();

    return 0;
}