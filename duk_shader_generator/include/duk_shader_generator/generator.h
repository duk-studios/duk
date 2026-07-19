/// generator.h

#ifndef DUK_SHADER_GENERATOR_GENERATOR_H
#define DUK_SHADER_GENERATOR_GENERATOR_H

#include <duk_macros/macros.h>
#include <duk_rhi/runtime_shader_data_source.h>
#include <duk_rhi/shader_compiler.h>
#include <duk_rhi/shader_reflection.h>

#include <expected>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace duk::shader_generator {

struct Options;

// Compiles shaders and writes all generated source files to disk.
void generate(const Options& options);

}// namespace duk::shader_generator

#endif// DUK_SHADER_GENERATOR_GENERATOR_H
