/// shader_generator.cc

#include <duk_shader_generator/file_generators/shader_data_source_generator.h>

#include <duk_rhi/shader_compiler.h>

#include <catch2/catch_test_macros.hpp>

namespace {

// ---------------------------------------------------------------------------
// Shader sources used across tests
// ---------------------------------------------------------------------------

constexpr std::string_view kVertWithPosition = R"glsl(
#version 450
layout(location = 0) in vec3 inPosition;
void main() {
    gl_Position = vec4(inPosition, 1.0);
}
)glsl";

constexpr std::string_view kFragWithSamplerAndUBO = R"glsl(
#version 450
uniform sampler2D albedo;
uniform MaterialBlock {
    vec4 color;
    float roughness;
} material;
layout(location = 0) out vec4 outColor;
void main() {
    outColor = texture(albedo, vec2(0.0)) * material.color * material.roughness;
}
)glsl";

constexpr std::string_view kMinimalVert = R"glsl(
#version 450
void main() {
    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
}
)glsl";

constexpr std::string_view kMinimalFrag = R"glsl(
#version 450
layout(location = 0) out vec4 outColor;
void main() {
    outColor = vec4(1.0);
}
)glsl";

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

duk::rhi::RuntimeShaderDataSource compile_glsl(
        const std::unordered_map<duk::rhi::ShaderModule::Bits, std::string_view>& sources) {
    duk::rhi::ShaderCompilerCreateInfo info{};
    std::unordered_map<duk::rhi::ShaderModule::Bits, std::string> owned;
    for (const auto& [stage, src]: sources) {
        owned[stage] = std::string(src);
    }
    return duk::rhi::compile(info, owned);
}

bool contains(const std::string& text, const std::string& needle) {
    return text.find(needle) != std::string::npos;
}

}// anonymous namespace

// ---------------------------------------------------------------------------
// generate_shader_data_source — class structure in header
// ---------------------------------------------------------------------------

TEST_CASE("generate_shader_data_source header structure", "[shader_generator]") {
    const auto source = compile_glsl({
            {duk::rhi::ShaderModule::VERTEX,   kVertWithPosition},
            {duk::rhi::ShaderModule::FRAGMENT, kFragWithSamplerAndUBO},
    });

    const auto files = duk::shader_generator::generate_shader_data_source("test_shader", "my::ns", source);
    const auto& h = files.headerContent;

    SECTION("include guard is present") {
        CHECK(contains(h, "#ifndef DUK_GENERATED_TEST_SHADER_SHADER_DATA_SOURCE_H"));
        CHECK(contains(h, "#define DUK_GENERATED_TEST_SHADER_SHADER_DATA_SOURCE_H"));
        CHECK(contains(h, "#endif"));
    }

    SECTION("namespace is applied") {
        CHECK(contains(h, "namespace my::ns"));
    }

    SECTION("class declaration inherits ShaderDataSource") {
        CHECK(contains(h, "class TestShaderShaderDataSource : public duk::rhi::ShaderDataSource"));
    }

    SECTION("Binding enum is generated") {
        CHECK(contains(h, "enum class Binding : uint32_t"));
    }

    SECTION("binding enum contains correct enumerators") {
        CHECK(contains(h, "ALBEDO"));
        CHECK(contains(h, "MATERIAL"));
    }

    SECTION("inner buffer struct is generated") {
        CHECK(contains(h, "struct MaterialData"));
        CHECK(contains(h, "glm::vec4 color"));
        CHECK(contains(h, "float roughness"));
    }

    SECTION("ShaderDataSource overrides are declared") {
        CHECK(contains(h, "module_mask()"));
        CHECK(contains(h, "shader_module_spir_v_code("));
        CHECK(contains(h, "shader_modules()"));
        CHECK(contains(h, "binding_layout()"));
        CHECK(contains(h, "vertex_layout()"));
        CHECK(contains(h, "calculate_hash()"));
    }
}

// ---------------------------------------------------------------------------
// generate_shader_data_source — source content
// ---------------------------------------------------------------------------

TEST_CASE("generate_shader_data_source source content", "[shader_generator]") {
    const auto source = compile_glsl({
            {duk::rhi::ShaderModule::VERTEX,   kVertWithPosition},
            {duk::rhi::ShaderModule::FRAGMENT, kFragWithSamplerAndUBO},
    });

    const auto files = duk::shader_generator::generate_shader_data_source("test_shader", "my::ns", source);
    const auto& cpp = files.sourceContent;

    SECTION("namespace is applied") {
        CHECK(contains(cpp, "namespace my::ns"));
    }

    SECTION("SPIR-V arrays are embedded for both stages") {
        CHECK(contains(cpp, "kVertexSpirV"));
        CHECK(contains(cpp, "kFragmentSpirV"));
    }

    SECTION("static binding layout is initialized") {
        CHECK(contains(cpp, "kBindingLayout"));
    }

    SECTION("static vertex layout is initialized") {
        CHECK(contains(cpp, "kVertexLayout"));
    }

    SECTION("method bodies reference the static data") {
        CHECK(contains(cpp, "TestShaderShaderDataSource::module_mask"));
        CHECK(contains(cpp, "TestShaderShaderDataSource::binding_layout"));
        CHECK(contains(cpp, "TestShaderShaderDataSource::vertex_layout"));
        CHECK(contains(cpp, "TestShaderShaderDataSource::calculate_hash"));
    }

    SECTION("hash is embedded as a literal") {
        const auto hash = source.hash();
        CHECK(contains(cpp, std::to_string(hash) + "ULL"));
    }
}

// ---------------------------------------------------------------------------
// generate_shader_data_source — shader with no bindings
// ---------------------------------------------------------------------------

TEST_CASE("generate_shader_data_source with no bindings", "[shader_generator]") {
    const auto source = compile_glsl({
            {duk::rhi::ShaderModule::VERTEX,   kMinimalVert},
            {duk::rhi::ShaderModule::FRAGMENT, kMinimalFrag},
    });

    const auto files = duk::shader_generator::generate_shader_data_source("empty_shader", "duk::test", source);
    const auto& h = files.headerContent;

    SECTION("Binding enum is still present but empty") {
        CHECK(contains(h, "enum class Binding : uint32_t"));
    }

    SECTION("no inner structs are generated") {
        CHECK_FALSE(contains(h, "struct "));
    }

    SECTION("class name is correct") {
        CHECK(contains(h, "class EmptyShaderShaderDataSource"));
    }
}

// ---------------------------------------------------------------------------
// generate_shader_data_source — output namespace variations
// ---------------------------------------------------------------------------

TEST_CASE("generate_shader_data_source namespace variations", "[shader_generator]") {
    const auto source = compile_glsl({
            {duk::rhi::ShaderModule::VERTEX,   kMinimalVert},
            {duk::rhi::ShaderModule::FRAGMENT, kMinimalFrag},
    });

    SECTION("flat namespace") {
        const auto files = duk::shader_generator::generate_shader_data_source("my_shader", "engine", source);
        CHECK(contains(files.headerContent, "namespace engine"));
        CHECK(contains(files.sourceContent, "namespace engine"));
    }

    SECTION("nested namespace") {
        const auto files = duk::shader_generator::generate_shader_data_source("my_shader", "engine::shaders", source);
        CHECK(contains(files.headerContent, "namespace engine::shaders"));
        CHECK(contains(files.sourceContent, "namespace engine::shaders"));
    }
}



