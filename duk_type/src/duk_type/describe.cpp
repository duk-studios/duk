//
// Created by rov on 06/12/2025.
//

#include <duk_type/describe.h>
#include <algorithm>
#include <cctype>
#include <regex>
#include <string>

namespace duk::type {

static std::string normalize_type_name(std::string s) {
    // 1. Trim whitespace
    s.erase(std::remove_if(s.begin(), s.end(),
                           [](unsigned char c) {
                               return std::isspace(c);
                           }),
            s.end());

    // 2. Normalize unsigned integer suffixes in template args:
    //    4u, 4ul, 4ull, 4U, 4UL, 4ULL  →  4
    {
        static const std::regex unsigned_suffix(R"((\d+)(u|ul|ull|U|UL|ULL)(?=[>,\]]|$))");
        s = std::regex_replace(s, unsigned_suffix, "$1");
    }

    // 3. Normalize spacing after commas: "<float,4>" instead of "<float,4>"
    {
        static const std::regex comma_space(R"(,\s*)");
        s = std::regex_replace(s, comma_space, ",");
    }

    // 4. Normalize closing angle brackets: ">>" is fine, but remove spaces
    {
        static const std::regex angle_space(R"(\s*>)");
        s = std::regex_replace(s, angle_space, ">");
    }

    return s;
}

std::string name_of(const boost::typeindex::type_index& info) {
    return normalize_type_name(info.pretty_name());
}

}// namespace duk::type