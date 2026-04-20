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
    // 1. Strip MSVC-style type-kind prefixes that appear before type names or
    //    template arguments (e.g. "class std::vector<…>", "struct foo::Bar",
    //    "enum class foo::DummyEnum").  We must do this *before* collapsing
    //    whitespace, otherwise the prefix gets concatenated directly onto the
    //    type name (e.g. "classstd::vector<int>").
    {
        static const std::regex type_prefix(R"(\b(class|struct|enum|union) )");
        s = std::regex_replace(s, type_prefix, "");
    }

    // 2. Trim remaining whitespace
    s.erase(std::remove_if(s.begin(), s.end(),
                           [](unsigned char c) {
                               return std::isspace(c);
                           }),
            s.end());

    // 3. Normalize unsigned integer suffixes in template args:
    //    4u, 4ul, 4ull, 4U, 4UL, 4ULL  →  4
    {
        static const std::regex unsigned_suffix(R"((\d+)(u|ul|ull|U|UL|ULL)(?=[>,\]]|$))");
        s = std::regex_replace(s, unsigned_suffix, "$1");
    }

    // 4. Normalize spacing after commas:
    {
        static const std::regex comma_space(R"(,\s*)");
        s = std::regex_replace(s, comma_space, ",");
    }

    // 5. Normalize closing angle brackets: ">>" is fine, but remove spaces
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