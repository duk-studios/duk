/// 04/11/2023
/// file_generator.cpp

#include <duk_cli/file_generator.h>
#include <fstream>

namespace duk::cli {

namespace detail {

static std::string include_guard_name(std::string name) {
    for (auto& c : name) {
        c = std::toupper(c);
    }
    return name + "_H";
}

}

void generate_include_guard_start(std::ostringstream& oss, const std::string& fileName) {
    const auto includeGuard = detail::include_guard_name(fileName);
    oss << "#ifndef " << includeGuard << std::endl;
    oss << "#define " << includeGuard << std::endl;
}

void generate_include_guard_end(std::ostringstream& oss, const std::string& fileName) {
    const auto includeGuard = detail::include_guard_name(fileName);
    oss << "#endif // " << includeGuard << std::endl;
}

void generate_include_directives(std::ostringstream& oss, std::span<const std::string> includes) {
    for (const auto& include: includes) {
        oss << "#include <" << include << '>' << std::endl;
    }
}

void generate_namespace_start(std::ostringstream& oss, const std::string& name) {
    oss << "namespace " << name << " {" << std::endl;
}

void generate_namespace_end(std::ostringstream& oss, const std::string& name) {
    oss << "} // namespace " << name << std::endl;
}

void write_file(const std::string& content, const std::string& filepath) {
    std::ofstream file(filepath);

    if (!file) {
        throw std::runtime_error("failed to write file at: " + filepath);
    }

    file << content;
}

}
