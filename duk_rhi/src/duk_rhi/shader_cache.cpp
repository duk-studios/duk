/// shader_cache.cpp

#include <duk_rhi/shader_cache.h>

#include <fstream>
#include <string>

namespace duk::rhi {

ShaderDiskCache::ShaderDiskCache(std::filesystem::path directory)
    : m_directory(std::move(directory)) {
}

std::expected<std::vector<uint8_t>, ShaderCacheError> ShaderDiskCache::load(duk::hash::Hash hash) const noexcept {
    try {
        auto path = entry_path(hash);
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file) {
            return std::unexpected(ShaderCacheError::NOT_FOUND);
        }
        auto size = static_cast<std::streamsize>(file.tellg());
        file.seekg(0);
        std::vector<uint8_t> data(static_cast<size_t>(size));
        if (!file.read(reinterpret_cast<char*>(data.data()), size)) {
            return std::unexpected(ShaderCacheError::IO_ERROR);
        }
        return data;
    } catch (...) {
        return std::unexpected(ShaderCacheError::IO_ERROR);
    }
}

void ShaderDiskCache::store(duk::hash::Hash hash, const std::vector<uint8_t>& spirv) noexcept {
    try {
        std::filesystem::create_directories(m_directory);
        auto path = entry_path(hash);
        std::ofstream file(path, std::ios::binary | std::ios::trunc);
        if (file) {
            file.write(reinterpret_cast<const char*>(spirv.data()), static_cast<std::streamsize>(spirv.size()));
        }
    } catch (...) {
    }
}

std::filesystem::path ShaderDiskCache::entry_path(duk::hash::Hash hash) const {
    return m_directory / (std::to_string(hash) + ".spv");
}

}// namespace duk::rhi
