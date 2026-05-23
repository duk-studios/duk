/// shader_compiler.cpp

#include <duk_rhi/shader_compiler.h>

#include <duk_hash/hash_combine.h>
#include <duk_log/log.h>

#include <shaderc/shaderc.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace duk::rhi {

namespace {

shaderc_shader_kind to_shaderc_kind(ShaderModule::Bits stage) {
    switch (stage) {
        case ShaderModule::VERTEX:
            return shaderc_vertex_shader;
        case ShaderModule::TESSELLATION_CONTROL:
            return shaderc_tess_control_shader;
        case ShaderModule::TESSELLATION_EVALUATION:
            return shaderc_tess_evaluation_shader;
        case ShaderModule::GEOMETRY:
            return shaderc_geometry_shader;
        case ShaderModule::FRAGMENT:
            return shaderc_fragment_shader;
        case ShaderModule::COMPUTE:
            return shaderc_compute_shader;
        default:
            throw std::invalid_argument("ShaderCompiler: unknown shader stage " + std::to_string(static_cast<int>(stage)));
    }
}

shaderc_optimization_level to_shaderc_optimization(OptimizationLevel level) {
    switch (level) {
        case OptimizationLevel::NONE:
            return shaderc_optimization_level_zero;
        case OptimizationLevel::PERFORMANCE:
            return shaderc_optimization_level_performance;
        case OptimizationLevel::SIZE:
            return shaderc_optimization_level_size;
        default:
            throw std::invalid_argument("ShaderCompiler: unknown optimization level " + std::to_string(static_cast<int>(level)));
    }
}

class FileIncluder : public shaderc::CompileOptions::IncluderInterface {
public:
    void add_search_path(std::filesystem::path path) {
        m_searchPaths.push_back(std::move(path));
    }

    void add_virtual_include(std::string path, std::string source) {
        m_virtualIncludes.emplace(std::move(path), std::move(source));
    }

    shaderc_include_result* GetInclude(const char* requested_source, shaderc_include_type /*type*/, const char* /*requesting_source*/, size_t /*include_depth*/) override {
        auto it = m_virtualIncludes.find(requested_source);
        if (it != m_virtualIncludes.end()) {
            auto* result = new shaderc_include_result{};
            auto* content = new std::string(it->second);
            auto* sourceName = new std::string(it->first);
            result->source_name = sourceName->c_str();
            result->source_name_length = sourceName->size();
            result->content = content->c_str();
            result->content_length = content->size();
            result->user_data = new std::pair<std::string*, std::string*>(sourceName, content);
            return result;
        }

        std::filesystem::path requestedPath(requested_source);

        for (const auto& dir: m_searchPaths) {
            auto candidate = dir / requestedPath;
            if (std::filesystem::exists(candidate)) {
                std::ifstream file(candidate, std::ios::binary);
                if (file) {
                    std::ostringstream ss;
                    ss << file.rdbuf();
                    auto* result = new shaderc_include_result{};
                    auto* content = new std::string(ss.str());
                    auto* sourceName = new std::string(candidate.generic_string());
                    result->source_name = sourceName->c_str();
                    result->source_name_length = sourceName->size();
                    result->content = content->c_str();
                    result->content_length = content->size();
                    result->user_data = new std::pair<std::string*, std::string*>(sourceName, content);
                    return result;
                }
            }
        }

        auto* result = new shaderc_include_result{};
        result->source_name = "";
        result->source_name_length = 0;
        result->content = "file not found";
        result->content_length = 14;
        result->user_data = nullptr;
        return result;
    }

    void ReleaseInclude(shaderc_include_result* data) override {
        if (data->user_data) {
            auto* pair = static_cast<std::pair<std::string*, std::string*>*>(data->user_data);
            delete pair->first;
            delete pair->second;
            delete pair;
        }
        delete data;
    }

private:
    std::unordered_map<std::string, std::string> m_virtualIncludes;
    std::vector<std::filesystem::path> m_searchPaths;
};

}// anonymous namespace

struct ShaderCompiler::Impl {
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    FileIncluder* includer = nullptr;
    std::shared_ptr<ShaderCache> cache;
};

ShaderCompiler::ShaderCompiler(const ShaderCompilerCreateInfo& createInfo)
    : m_impl(std::make_unique<Impl>()) {
    switch (createInfo.api) {
        case API::VULKAN:
            m_impl->options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_1);
            break;
        default:
            duk::log::fatal("ShaderCompiler: unsupported API {}", static_cast<int>(createInfo.api));
    }

    m_impl->options.SetAutoBindUniforms(true);
    m_impl->options.SetAutoMapLocations(true);
    m_impl->options.SetOptimizationLevel(to_shaderc_optimization(createInfo.optimizationLevel));

    auto includer = std::make_unique<FileIncluder>();
    m_impl->includer = includer.get();
    for (const auto& dir: createInfo.includeDirectories) {
        m_impl->includer->add_search_path(dir);
    }
    for (const auto& [path, source]: createInfo.virtualIncludes) {
        m_impl->includer->add_virtual_include(path, source);
    }

    m_impl->options.SetIncluder(std::move(includer));

    m_impl->cache = createInfo.cache;
}

ShaderCompiler::~ShaderCompiler() = default;

ShaderCompiler::ShaderCompiler(ShaderCompiler&&) noexcept = default;

ShaderCompiler& ShaderCompiler::operator=(ShaderCompiler&&) noexcept = default;

void ShaderCompiler::add_include_directory(std::filesystem::path directory) {
    m_impl->includer->add_search_path(std::move(directory));
}

void ShaderCompiler::add_virtual_include(std::string path, std::string source) {
    m_impl->includer->add_virtual_include(std::move(path), std::move(source));
}

void ShaderCompiler::set_cache(std::shared_ptr<ShaderCache> cache) {
    m_impl->cache = std::move(cache);
}

std::expected<std::vector<uint8_t>, ShaderCompileError> ShaderCompiler::compile(std::string_view source, ShaderModule::Bits stage, const char* debugName) const noexcept {
    try {
        auto preprocessed = m_impl->compiler.PreprocessGlsl(source.data(), source.size(), to_shaderc_kind(stage), debugName, m_impl->options);

        if (preprocessed.GetCompilationStatus() != shaderc_compilation_status_success) {
            return std::unexpected(ShaderCompileError{ShaderCompileError::Code::PREPROCESS_FAILED, preprocessed.GetErrorMessage()});
        }

        duk::hash::Hash hash = 0;
        duk::hash::hash_combine(hash, preprocessed.cbegin(), preprocessed.cend());
        duk::hash::hash_combine(hash, static_cast<uint32_t>(stage));

        if (m_impl->cache) {
            auto cached = m_impl->cache->load(hash);
            if (cached) {
                return std::move(*cached);
            }
        }

        auto result = m_impl->compiler.CompileGlslToSpv(source.data(), source.size(), to_shaderc_kind(stage), debugName, m_impl->options);

        if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
            return std::unexpected(ShaderCompileError{ShaderCompileError::Code::COMPILATION_FAILED, result.GetErrorMessage()});
        }

        const auto* begin = reinterpret_cast<const uint8_t*>(result.cbegin());
        const auto* end = reinterpret_cast<const uint8_t*>(result.cend());
        std::vector spirv(begin, end);

        if (m_impl->cache) {
            m_impl->cache->store(hash, spirv);
        }

        return spirv;
    } catch (const std::exception& e) {
        return std::unexpected(ShaderCompileError{ShaderCompileError::Code::COMPILATION_FAILED, e.what()});
    } catch (...) {
        return std::unexpected(ShaderCompileError{ShaderCompileError::Code::COMPILATION_FAILED, "unknown exception"});
    }
}

}// namespace duk::rhi
