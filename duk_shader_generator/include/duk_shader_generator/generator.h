/// generator.h

#ifndef DUK_SHADER_GENERATOR_GENERATOR_H
#define DUK_SHADER_GENERATOR_GENERATOR_H

#include <duk_macros/macros.h>
#include <duk_rhi/runtime_shader_data_source.h>

namespace duk::shader_generator {

struct Options;

/// Reads the GLSL file paths from Options, compiles them, and returns a RuntimeShaderDataSource.
DUK_NO_DISCARD duk::rhi::RuntimeShaderDataSource compile(const Options& options);

/// Compiles the shaders described by Options and writes the generated source files to disk.
void generate(const Options& options);

}// namespace duk::shader_generator

#endif// DUK_SHADER_GENERATOR_GENERATOR_H
