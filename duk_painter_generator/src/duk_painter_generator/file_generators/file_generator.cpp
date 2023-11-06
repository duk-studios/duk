/// 04/11/2023
/// file_generator.cpp

#include <duk_painter_generator/file_generators/file_generator.h>
#include <duk_painter_generator/types.h>

#include <fstream>

namespace duk::painter_generator {

namespace detail {

static std::string include_guard_name(std::string name) {
    for (auto& c : name) {
        c = std::toupper(c);
    }
    return "DUK_RENDERER_" + name + "_H";
}

}

FileGenerator::~FileGenerator() = default;

void FileGenerator::generate_include_guard_start(std::ostringstream& oss, const std::string& fileName) {
    const auto includeGuard = detail::include_guard_name(fileName);
    oss << "#ifndef " << includeGuard << std::endl;
    oss << "#define " << includeGuard << std::endl;
}

void FileGenerator::generate_include_guard_end(std::ostringstream& oss, const std::string& fileName) {
    const auto includeGuard = detail::include_guard_name(fileName);
    oss << "#endif // " << includeGuard << std::endl;
}

void FileGenerator::generate_include_directives(std::ostringstream& oss, std::span<const std::string> includes) {
    for (const auto& include: includes) {
        oss << "#include <" << include << '>' << std::endl;
    }
}

void FileGenerator::generate_namespace_start(std::ostringstream& oss, const std::string& painterName) {
    oss << "namespace duk::renderer";
    if (!painterName.empty()) {
        oss << "::" << painterName;
    }
    oss << " {" << std::endl;
}

void FileGenerator::generate_namespace_end(std::ostringstream& oss, const std::string& painterName) {
    oss << "} // namespace duk::renderer";
    if (!painterName.empty()) {
        oss << "::" << painterName;
    }
    oss << std::endl;
}

void FileGenerator::write_file(const std::string& content, const std::string& filepath) {
    std::ofstream file(filepath);

    if (!file) {
        throw std::runtime_error("failed to write file at: " + filepath);
    }

    file << content;
}

}
