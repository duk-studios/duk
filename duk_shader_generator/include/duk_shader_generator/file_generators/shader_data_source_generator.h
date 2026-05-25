/// shader_data_source_generator.h

#ifndef DUK_SHADER_GENERATOR_SHADER_DATA_SOURCE_GENERATOR_H
#define DUK_SHADER_GENERATOR_SHADER_DATA_SOURCE_GENERATOR_H

#include <duk_macros/macros.h>
#include <duk_rhi/runtime_shader_data_source.h>

#include <string>

namespace duk::shader_generator {

struct Options;

/// The generated header and source file contents as strings.
struct GeneratedFiles {
    std::string headerContent;
    std::string sourceContent;
};

/// Generates the header and source text for a ShaderDataSource subclass.
DUK_NO_DISCARD GeneratedFiles generate_shader_data_source(
        const std::string& shaderName,
        const std::string& outputNamespace,
        const rhi::RuntimeShaderDataSource& source);

/// Calls generate_shader_data_source and writes the two output files to disk.
void write_shader_data_source(const Options& options, const rhi::RuntimeShaderDataSource& source);

}// namespace duk::shader_generator

#endif// DUK_SHADER_GENERATOR_SHADER_DATA_SOURCE_GENERATOR_H
