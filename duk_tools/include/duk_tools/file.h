/// 05/10/2023
/// file.h

#ifndef DUK_TOOLS_FILE_H
#define DUK_TOOLS_FILE_H

#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace duk::tools {

template<typename T>
std::vector<uint8_t> load_text(const T& filepath) {
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

template<typename T>
std::vector<uint8_t> load_bytes(const T& filepath) {
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

template<typename T>
std::vector<uint8_t> save_bytes(const T& filepath, const std::vector<uint8_t>& bytes) {
    std::ofstream file(filepath, std::ios::ate | std::ios::binary);

    if (!file) {
        std::ostringstream oss;
        oss << "failed to save file at: " << filepath;
        throw std::runtime_error(oss.str());
    }

    std::vector<uint8_t> buffer(bytes.begin(), bytes.end());
    file.write(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(buffer.size()));
}
}// namespace duk::tools

#endif// DUK_TOOLS_FILE_H
