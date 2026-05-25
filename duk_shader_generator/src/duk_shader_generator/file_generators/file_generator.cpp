/// 04/11/2023
/// file_generator.cpp

#include <duk_shader_generator/file_generators/file_generator.h>

#include <cctype>
#include <fstream>

namespace duk::shader_generator {

namespace detail {

static std::string include_guard_name(const std::string& fileName) {
    std::string guard = "DUK_GENERATED_";
    for (char c: fileName) {
        guard += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    guard += "_H";
    return guard;
}

}// namespace detail

FileGenerator::~FileGenerator() = default;

void FileGenerator::generate_include_guard_start(std::ostringstream& oss, const std::string& fileName) {
    const auto guard = detail::include_guard_name(fileName);
    oss << "#ifndef " << guard << '\n';
    oss << "#define " << guard << '\n';
}

void FileGenerator::generate_include_guard_end(std::ostringstream& oss, const std::string& fileName) {
    const auto guard = detail::include_guard_name(fileName);
    oss << "#endif// " << guard << '\n';
}

void FileGenerator::generate_include_directives(std::ostringstream& oss, std::span<const std::string> includes) {
    for (const auto& include: includes) {
        oss << "#include <" << include << ">\n";
    }
}

void FileGenerator::generate_namespace_start(std::ostringstream& oss, const std::string& namespaceName) {
    if (namespaceName.empty()) {
        return;
    }
    oss << "namespace " << namespaceName << " {\n";
}

void FileGenerator::generate_namespace_end(std::ostringstream& oss, const std::string& namespaceName) {
    if (namespaceName.empty()) {
        return;
    }
    oss << "}// namespace " << namespaceName << '\n';
}

void FileGenerator::write_file(const std::string& content, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file) {
        throw std::runtime_error("failed to write file at: " + filepath);
    }
    file << content;
}

}// namespace duk::shader_generator
