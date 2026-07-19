/// shader_data_source_file_generator.h

#ifndef DUK_SHADER_GENERATOR_SHADER_DATA_SOURCE_FILE_GENERATOR_H
#define DUK_SHADER_GENERATOR_SHADER_DATA_SOURCE_FILE_GENERATOR_H

#include <duk_shader_generator/generator.h>
#include <duk_shader_generator/options.h>

namespace duk::shader_generator {

// Generates the header and source files for the shader data source class
// derived from the given options and compiled runtime shader data.
void generate_shader_data_source_files(const Options& options, const rhi::RuntimeShaderDataSource& shaderDataSource);

}// namespace duk::shader_generator

#endif// DUK_SHADER_GENERATOR_SHADER_DATA_SOURCE_FILE_GENERATOR_H
