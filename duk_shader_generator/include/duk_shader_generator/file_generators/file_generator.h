/// file_generator.h

#ifndef DUK_SHADER_GENERATOR_FILE_GENERATOR_H
#define DUK_SHADER_GENERATOR_FILE_GENERATOR_H

#include <span>
#include <sstream>
#include <string>

namespace duk::shader_generator {

class FileGenerator {
public:
    virtual ~FileGenerator();

protected:
    static void generate_include_guard_start(std::ostringstream& oss, const std::string& fileName);

    static void generate_include_guard_end(std::ostringstream& oss, const std::string& fileName);

    static void generate_include_directives(std::ostringstream& oss, std::span<const std::string> includes);

    static void generate_namespace_start(std::ostringstream& oss, const std::string& namespaceName);

    static void generate_namespace_end(std::ostringstream& oss, const std::string& namespaceName);

    static void write_file(const std::string& content, const std::string& filepath);
};

}// namespace duk::shader_generator

#endif// DUK_SHADER_GENERATOR_FILE_GENERATOR_H
