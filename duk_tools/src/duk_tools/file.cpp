/// 05/10/2023
/// file.cpp

#include <duk_tools/file.h>

#include <zlib.h>

#include <fstream>
#include <sstream>
#include <fmt/format.h>

namespace duk::tools {

std::vector<uint8_t> load_compressed_bytes(const char* filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);

    if (!file) {
        throw std::runtime_error(fmt::format("Failed to load compressed file '{}', does not exist", filepath));
    }

    file.seekg(0);

    // read decompressed size
    uint64_t decompressedSize = 0;
    file.read(reinterpret_cast<char*>(&decompressedSize), sizeof(uint64_t));

    uint64_t compressedSize = 0;
    file.read(reinterpret_cast<char*>(&compressedSize), sizeof(uint64_t));

    // allocate compressed data
    std::vector<uint8_t> compressedData(compressedSize);

    // read compressed data
    file.read(reinterpret_cast<char*>(compressedData.data()), static_cast<std::streamsize>(compressedData.size()));

    // allocate decompressed data
    std::vector<uint8_t> decompressedData(decompressedSize);

    // decompress data
    int result = uncompress(decompressedData.data(), reinterpret_cast<uLongf*>(&decompressedSize), compressedData.data(), compressedSize);
    if (result != Z_OK) {
        throw std::runtime_error(fmt::format("Failed to decompress compressed file '{}', error code {}", filepath, result));
    }

    return decompressedData;
}

std::vector<uint8_t> load_compressed_bytes(const std::string_view& filepath) {
    return load_compressed_bytes(filepath.data());
}

std::vector<uint8_t> load_compressed_bytes(const std::filesystem::path& filepath) {
    return load_compressed_bytes(filepath.string().c_str());
}

void save_compressed_bytes(const char* filepath, const void* data, size_t size) {
    uLongf compressedSize = compressBound(size);
    std::vector<uint8_t> compressedData(compressedSize);

    int result = compress(compressedData.data(), &compressedSize, static_cast<const Bytef*>(data), size);
    if (result != Z_OK) {
        throw std::runtime_error(fmt::format("Failed to compress file '{}', error code {}", filepath, result));
    }

    std::ofstream file(filepath, std::ios::binary | std::ios::trunc);
    if (!file) {
        throw std::runtime_error(fmt::format("Failed to save compressed file '{}', does not exist", filepath));
    }

    // write decompressed size at beginning
    uint64_t sizeu64 = static_cast<uint64_t>(size);
    file.write(reinterpret_cast<char*>(&sizeu64), sizeof(uint64_t));

    // write compressed size
    uint64_t compressedSizeu64 = static_cast<uint64_t>(compressedSize);
    file.write(reinterpret_cast<char*>(&compressedSizeu64), sizeof(uint64_t));

    // write compressed data
    file.write(reinterpret_cast<char*>(compressedData.data()), compressedSize);
}

void save_compressed_bytes(const std::string_view& filepath, const void* data, size_t size) {
    save_compressed_bytes(filepath.data(), data, size);
}

void save_compressed_bytes(const std::filesystem::path& filepath, const void* data, size_t size) {
    save_compressed_bytes(filepath.string().c_str(), data, size);
}

}// namespace duk::tools