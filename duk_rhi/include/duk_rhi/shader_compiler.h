/// shader_compiler.h

#ifndef DUK_RHI_SHADER_COMPILER_H
#define DUK_RHI_SHADER_COMPILER_H

#include <duk_rhi/instance.h>
#include <duk_rhi/runtime_shader_data_source.h>
#include <duk_rhi/shader_cache.h>

#include <cstdint>
#include <expected>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace duk::rhi {

enum class OptimizationLevel {
    NONE,
    PERFORMANCE,
    SIZE
};

struct ShaderCompileError {
    enum class Code {
        PREPROCESS_FAILED,
        COMPILATION_FAILED
    };

    Code code;
    std::string message;
};

struct ShaderCompilerCreateInfo {
    API api = API::VULKAN;
    OptimizationLevel optimizationLevel = OptimizationLevel::NONE;
    std::vector<std::filesystem::path> includeDirectories;
    std::unordered_map<std::string, std::string> virtualIncludes;
    std::shared_ptr<ShaderCache> cache;
};

class ShaderCompiler {
public:
    explicit ShaderCompiler(const ShaderCompilerCreateInfo& createInfo);

    ~ShaderCompiler();

    ShaderCompiler(const ShaderCompiler&) = delete;
    ShaderCompiler& operator=(const ShaderCompiler&) = delete;

    ShaderCompiler(ShaderCompiler&&) noexcept;
    ShaderCompiler& operator=(ShaderCompiler&&) noexcept;

    void add_include_directory(std::filesystem::path directory);

    void add_virtual_include(std::string path, std::string source);

    void set_cache(std::shared_ptr<ShaderCache> cache);

    DUK_NO_DISCARD std::expected<std::vector<uint8_t>, ShaderCompileError> compile(std::string_view source, ShaderModule::Bits stage, const char* debugName) const noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

/// Compiles multiple GLSL stage sources using an existing ShaderCompiler instance.
DUK_NO_DISCARD RuntimeShaderDataSource compile(
        const ShaderCompiler& compiler,
        const std::unordered_map<ShaderModule::Bits, std::string>& glslSources);

/// Compiles multiple GLSL stage sources, constructing a ShaderCompiler from the given create info.
DUK_NO_DISCARD RuntimeShaderDataSource compile(
        const ShaderCompilerCreateInfo& createInfo,
        const std::unordered_map<ShaderModule::Bits, std::string>& glslSources);

}// namespace duk::rhi

#endif// DUK_RHI_SHADER_COMPILER_H
