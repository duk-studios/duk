/// 05/10/2023
/// file.cpp

#include <duk_tools/file.h>

#include <fstream>
#include <sstream>

namespace duk::tools {

std::vector<uint8_t> File::load_bytes(const char* filepath) {
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);

    if (!file) {
        std::ostringstream oss;
        oss << "failed to open file at: " << filepath;
        throw std::runtime_error(oss.str());
    }

    std::vector<uint8_t> buffer(file.tellg());
    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(buffer.size()));

    return buffer;
}

std::vector<char> File::load_text(const char* filepath) {
    std::ifstream file(filepath, std::ios_base::ate);

    if (!file) {
        std::ostringstream oss;
        oss << "failed to open file at: " << filepath;
        throw std::runtime_error(oss.str());
    }

    const auto size = file.tellg();
    std::vector<char> buffer(static_cast<std::streamsize>(size));
    file.seekg(0);
    file.read(buffer.data(), size);

    return buffer;
}

}// namespace duk::tools