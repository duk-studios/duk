/// 04/11/2023
/// file_generator.cpp

#include <duk_painter_generator/file_generators/file_generator.h>
#include <duk_painter_generator/types.h>

#include <fstream>

namespace duk::painter_generator {

FileGenerator::~FileGenerator() = default;

void FileGenerator::generate_include_directives(std::ostringstream& oss, std::span<const std::string> includes) {
    for (const auto& include: includes) {
        oss << "#include <" << include << '>' << std::endl;
    }
}

void FileGenerator::generate_namespace_start(std::ostringstream& oss, const std::string& painterName) {
    oss << "namespace duk::renderer::" << painterName << " {" << std::endl;
}

void FileGenerator::generate_namespace_end(std::ostringstream& oss, const std::string& painterName) {
    oss << "} // namespace duk::renderer::" << painterName << std::endl;
}

void FileGenerator::write_file(const std::string& content, const std::string& filepath) {
    std::ofstream file(filepath);

    if (!file) {
        throw std::runtime_error("failed to write file at: " + filepath);
    }

    file << content;
}

}