/// 04/11/2023
/// file_generator.h

#ifndef DUK_MATERIAL_GENERATOR_FILE_GENERATOR_H
#define DUK_MATERIAL_GENERATOR_FILE_GENERATOR_H

#include <duk_material_generator/reflector.h>

#include <span>
#include <sstream>
#include <vector>

namespace duk::material_generator {

class FileGenerator {
public:
    virtual ~FileGenerator();

protected:
    static void generate_include_guard_start(std::ostringstream& oss, const std::string& fileName);

    static void generate_include_guard_end(std::ostringstream& oss, const std::string& fileName);

    static void generate_include_directives(std::ostringstream& oss, std::span<const std::string> includes);

    static void generate_namespace_start(std::ostringstream& oss, const std::string& materialName);

    static void generate_namespace_end(std::ostringstream& oss, const std::string& materialName);

    static void write_file(const std::string& content, const std::string& filepath);
};

}// namespace duk::material_generator

#endif// DUK_MATERIAL_GENERATOR_FILE_GENERATOR_H
