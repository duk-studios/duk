/// shader_cache.h

#ifndef DUK_RHI_SHADER_CACHE_H
#define DUK_RHI_SHADER_CACHE_H

#include <duk_hash/hash_combine.h>
#include <duk_macros/macros.h>

#include <cstdint>
#include <expected>
#include <filesystem>
#include <vector>

namespace duk::rhi {

enum class ShaderCacheError {
    NOT_FOUND,
    IO_ERROR
};

/// Abstract shader cache. Implementations are not required to be thread-safe;
/// callers sharing a cache instance across threads must synchronise externally.
class ShaderCache {
public:
    virtual ~ShaderCache() = default;

    DUK_NO_DISCARD virtual std::expected<std::vector<uint8_t>, ShaderCacheError> load(duk::hash::Hash hash) const noexcept = 0;

    virtual void store(duk::hash::Hash hash, const std::vector<uint8_t>& spirv) noexcept = 0;
};

/// Stores each SPIR-V binary as <directory>/<hash>.spv on disk.
class ShaderDiskCache : public ShaderCache {
public:
    explicit ShaderDiskCache(std::filesystem::path directory);

    ~ShaderDiskCache() override = default;

    std::expected<std::vector<uint8_t>, ShaderCacheError> load(duk::hash::Hash hash) const noexcept override;

    void store(duk::hash::Hash hash, const std::vector<uint8_t>& spirv) noexcept override;

private:
    std::filesystem::path entry_path(duk::hash::Hash hash) const;

    std::filesystem::path m_directory;
};

}// namespace duk::rhi

#endif// DUK_RHI_SHADER_CACHE_H
