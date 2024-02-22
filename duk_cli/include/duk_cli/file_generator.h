/// 04/11/2023
/// file_generator.h

#ifndef DUK_CLI_FILE_GENERATOR_H
#define DUK_CLI_FILE_GENERATOR_H

#include <duk_macros/macros.h>

#include <sstream>
#include <vector>
#include <span>

namespace duk::cli {

void generate_include_guard_start(std::ostringstream& oss, const std::string& fileName);

void generate_include_guard_end(std::ostringstream& oss, const std::string& fileName);

void generate_include_directives(std::ostringstream& oss, std::span<const std::string> includes);

void generate_namespace_start(std::ostringstream& oss, const std::string& name);

void generate_namespace_end(std::ostringstream& oss, const std::string& name);

void write_file(const std::string& content, const std::string& filepath);

template<typename T, typename P>
std::string generate_for_each(const T& container, P predicate, const char* separator = ",", bool onePerLine = true) {
    std::ostringstream oss;

    bool needsSeparator = false;
    for (const auto& element: container) {
        if (needsSeparator) {
            oss << separator;
            if (onePerLine) {
                oss << std::endl;
            }
        }
        needsSeparator = true;
        oss << predicate(element);
    }

    return oss.str();
}

}

#endif // DUK_CLI_FILE_GENERATOR_H
