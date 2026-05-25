/// runtime_shader_data_source.cc

#include <duk_rhi/runtime_shader_data_source.h>
#include <duk_rhi/shader_compiler.h>

#include <catch2/catch_test_macros.hpp>

#include <variant>

namespace {

// ---------------------------------------------------------------------------
// Shader sources
// ---------------------------------------------------------------------------

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

constexpr std::string_view kVertWithAttributes = R"glsl(
#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
void main() {
    gl_Position = vec4(inPosition, 1.0);
}
)glsl";

constexpr std::string_view kFragWithUBO = R"glsl(
#version 450
uniform TransformBlock {
    mat4 model;
    vec3 cameraPos;
    float time;
} transform;
layout(location = 0) out vec4 outColor;
void main() {
    outColor = vec4(transform.model[0].x, transform.cameraPos.x, transform.time, 1.0);
}
)glsl";

constexpr std::string_view kFragWithSampler = R"glsl(
#version 450
uniform sampler2D diffuseTexture;
layout(location = 0) out vec4 outColor;
void main() {
    outColor = texture(diffuseTexture, vec2(0.0));
}
)glsl";

constexpr std::string_view kVertWithSharedUBO = R"glsl(
#version 450
uniform CameraBlock {
    mat4 viewProj;
} camera;
void main() {
    gl_Position = camera.viewProj * vec4(0.0, 0.0, 0.0, 1.0);
}
)glsl";

constexpr std::string_view kFragWithSharedUBO = R"glsl(
#version 450
uniform CameraBlock {
    mat4 viewProj;
} camera;
layout(location = 0) out vec4 outColor;
void main() {
    outColor = vec4(camera.viewProj[0]);
}
)glsl";

constexpr std::string_view kVertWithTextureAndUBO = R"glsl(
#version 450
uniform sampler2D albedo;
uniform MaterialBlock {
    vec4 tint;
} material;
void main() {
    gl_Position = vec4(material.tint.r, 0.0, 0.0, 1.0);
}
)glsl";

constexpr std::string_view kFragWithTextureAndUBO = R"glsl(
#version 450
uniform sampler2D albedo;
uniform MaterialBlock {
    vec4 tint;
} material;
layout(location = 0) out vec4 outColor;
void main() {
    outColor = texture(albedo, vec2(0.0)) * material.tint;
}
)glsl";

constexpr std::string_view kComputeWithSSBO = R"glsl(
#version 450
buffer DataBlock {
    float values[];
} data;
layout(local_size_x = 64) in;
void main() {
    data.values[gl_GlobalInvocationID.x] = 1.0;
}
)glsl";

constexpr std::string_view kMinimalVertAlt = R"glsl(
#version 450
layout(location = 0) in vec4 inPosition;
void main() {
    gl_Position = inPosition;
}
)glsl";

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

duk::rhi::ShaderCompiler make_compiler() {
    duk::rhi::ShaderCompilerCreateInfo info{};
    info.api = duk::rhi::API::VULKAN;
    return duk::rhi::ShaderCompiler(info);
}

struct CompiledStage {
    duk::rhi::ShaderModule::Bits stage;
    std::string_view source;
    const char* debugName;
};

duk::rhi::RuntimeShaderDataSource make_source(std::initializer_list<CompiledStage> stages) {
    auto compiler = make_compiler();
    duk::rhi::RuntimeShaderDataSourceCreateInfo info;

    for (const auto& s: stages) {
        auto result = compiler.compile(s.source, s.stage, s.debugName);
        REQUIRE(result.has_value());

        switch (s.stage) {
            case duk::rhi::ShaderModule::VERTEX:                  info.vertexShaderCode = std::move(*result); break;
            case duk::rhi::ShaderModule::TESSELLATION_CONTROL:    info.tessellationControlShaderCode = std::move(*result); break;
            case duk::rhi::ShaderModule::TESSELLATION_EVALUATION: info.tessellationEvaluationShaderCode = std::move(*result); break;
            case duk::rhi::ShaderModule::GEOMETRY:                info.geometryShaderCode = std::move(*result); break;
            case duk::rhi::ShaderModule::FRAGMENT:                info.fragmentShaderCode = std::move(*result); break;
            case duk::rhi::ShaderModule::COMPUTE:                 info.computeShaderCode = std::move(*result); break;
            default: break;
        }
    }

    return duk::rhi::RuntimeShaderDataSource(info);
}

}// anonymous namespace

// ---------------------------------------------------------------------------
// Module mask
// ---------------------------------------------------------------------------

TEST_CASE("RuntimeShaderDataSource module mask", "[rhi][runtime_shader_data_source]") {
    SECTION("vertex+fragment") {
        auto source = make_source({{duk::rhi::ShaderModule::VERTEX, kMinimalVert, "test.vert"},
                                   {duk::rhi::ShaderModule::FRAGMENT, kMinimalFrag, "test.frag"}});
        const auto mask = source.module_mask();
        CHECK(mask & duk::rhi::ShaderModule::VERTEX);
        CHECK(mask & duk::rhi::ShaderModule::FRAGMENT);
        CHECK_FALSE(mask & duk::rhi::ShaderModule::COMPUTE);
    }

    SECTION("compute only") {
        auto source = make_source({{duk::rhi::ShaderModule::COMPUTE, kComputeWithSSBO, "test.comp"}});
        const auto mask = source.module_mask();
        CHECK(mask & duk::rhi::ShaderModule::COMPUTE);
        CHECK_FALSE(mask & duk::rhi::ShaderModule::VERTEX);
        CHECK_FALSE(mask & duk::rhi::ShaderModule::FRAGMENT);
    }
}

// ---------------------------------------------------------------------------
// Shader modules map
// ---------------------------------------------------------------------------

TEST_CASE("RuntimeShaderDataSource shader_modules", "[rhi][runtime_shader_data_source]") {
    SECTION("map contains exactly the compiled stages") {
        auto source = make_source({{duk::rhi::ShaderModule::VERTEX, kMinimalVert, "test.vert"},
                                   {duk::rhi::ShaderModule::FRAGMENT, kMinimalFrag, "test.frag"}});
        const auto& modules = source.shader_modules();
        CHECK(modules.size() == 2);
        CHECK(modules.count(duk::rhi::ShaderModule::VERTEX) == 1);
        CHECK(modules.count(duk::rhi::ShaderModule::FRAGMENT) == 1);
    }

    SECTION("shader_module_spir_v_code returns non-empty bytecode for a present stage") {
        auto source = make_source({{duk::rhi::ShaderModule::VERTEX, kMinimalVert, "test.vert"},
                                   {duk::rhi::ShaderModule::FRAGMENT, kMinimalFrag, "test.frag"}});
        CHECK_FALSE(source.shader_module_spir_v_code(duk::rhi::ShaderModule::VERTEX).empty());
        CHECK_FALSE(source.shader_module_spir_v_code(duk::rhi::ShaderModule::FRAGMENT).empty());
    }
}

// ---------------------------------------------------------------------------
// Vertex layout
// ---------------------------------------------------------------------------

TEST_CASE("RuntimeShaderDataSource vertex layout", "[rhi][runtime_shader_data_source]") {
    SECTION("no vertex inputs yields empty layout") {
        auto source = make_source({{duk::rhi::ShaderModule::VERTEX, kMinimalVert, "test.vert"},
                                   {duk::rhi::ShaderModule::FRAGMENT, kMinimalFrag, "test.frag"}});
        CHECK(source.vertex_layout().size() == 0);
    }

    SECTION("vec3 at location 0 and vec2 at location 1") {
        auto source = make_source({{duk::rhi::ShaderModule::VERTEX, kVertWithAttributes, "attrs.vert"},
                                   {duk::rhi::ShaderModule::FRAGMENT, kMinimalFrag, "test.frag"}});
        const auto& layout = source.vertex_layout();
        REQUIRE(layout.size() == 2);
        CHECK(layout.format_at(0) == duk::rhi::VertexInput::Format::VEC3);
        CHECK(layout.format_at(1) == duk::rhi::VertexInput::Format::VEC2);
    }

    SECTION("vertex layout is empty for a compute-only source") {
        auto source = make_source({{duk::rhi::ShaderModule::COMPUTE, kComputeWithSSBO, "test.comp"}});
        CHECK(source.vertex_layout().size() == 0);
    }
}

// ---------------------------------------------------------------------------
// Binding layout — image bindings
// ---------------------------------------------------------------------------

TEST_CASE("RuntimeShaderDataSource image binding", "[rhi][runtime_shader_data_source]") {
    SECTION("combined image sampler in fragment stage") {
        auto source = make_source({{duk::rhi::ShaderModule::VERTEX, kMinimalVert, "test.vert"},
                                   {duk::rhi::ShaderModule::FRAGMENT, kFragWithSampler, "sampler.frag"}});
        const auto& layout = source.binding_layout();
        REQUIRE(layout.size() == 1);

        const auto& desc = layout[0];
        CHECK(desc.name == "diffuseTexture");
        CHECK(desc.moduleMask & duk::rhi::ShaderModule::FRAGMENT);

        REQUIRE(std::holds_alternative<duk::rhi::ImageBindingDescription>(desc.binding));
        const auto& imgDesc = std::get<duk::rhi::ImageBindingDescription>(desc.binding);
        CHECK(imgDesc.type == duk::rhi::ImageBindingType::IMAGE_SAMPLER);
    }
}

// ---------------------------------------------------------------------------
// Binding layout — buffer bindings
// ---------------------------------------------------------------------------

TEST_CASE("RuntimeShaderDataSource uniform buffer binding", "[rhi][runtime_shader_data_source]") {
    SECTION("binding name, type and member count") {
        auto source = make_source({{duk::rhi::ShaderModule::VERTEX, kMinimalVert, "test.vert"},
                                   {duk::rhi::ShaderModule::FRAGMENT, kFragWithUBO, "ubo.frag"}});
        const auto& layout = source.binding_layout();
        REQUIRE(layout.size() == 1);

        const auto& desc = layout[0];
        CHECK(desc.name == "transform");
        CHECK(desc.moduleMask & duk::rhi::ShaderModule::FRAGMENT);

        REQUIRE(std::holds_alternative<duk::rhi::BufferBindingDescription>(desc.binding));
        const auto& bufDesc = std::get<duk::rhi::BufferBindingDescription>(desc.binding);
        CHECK(bufDesc.type == duk::rhi::BufferBindingType::UNIFORM_BUFFER);
        REQUIRE(bufDesc.members.size() == 3);
    }

    SECTION("member names") {
        auto source = make_source({{duk::rhi::ShaderModule::VERTEX, kMinimalVert, "test.vert"},
                                   {duk::rhi::ShaderModule::FRAGMENT, kFragWithUBO, "ubo.frag"}});
        const auto& bufDesc = std::get<duk::rhi::BufferBindingDescription>(source.binding_layout()[0].binding);
        CHECK(bufDesc.members[0].name == "model");
        CHECK(bufDesc.members[1].name == "cameraPos");
        CHECK(bufDesc.members[2].name == "time");
    }

    SECTION("member type names") {
        auto source = make_source({{duk::rhi::ShaderModule::VERTEX, kMinimalVert, "test.vert"},
                                   {duk::rhi::ShaderModule::FRAGMENT, kFragWithUBO, "ubo.frag"}});
        const auto& bufDesc = std::get<duk::rhi::BufferBindingDescription>(source.binding_layout()[0].binding);
        CHECK(bufDesc.members[0].typeName == "mat4");
        CHECK(bufDesc.members[1].typeName == "vec3");
        CHECK(bufDesc.members[2].typeName == "float");
    }

    SECTION("member offsets are non-decreasing") {
        auto source = make_source({{duk::rhi::ShaderModule::VERTEX, kMinimalVert, "test.vert"},
                                   {duk::rhi::ShaderModule::FRAGMENT, kFragWithUBO, "ubo.frag"}});
        const auto& bufDesc = std::get<duk::rhi::BufferBindingDescription>(source.binding_layout()[0].binding);
        for (size_t i = 1; i < bufDesc.members.size(); i++) {
            CHECK(bufDesc.members[i].offset > bufDesc.members[i - 1].offset);
        }
    }

    SECTION("buffer size is positive") {
        auto source = make_source({{duk::rhi::ShaderModule::VERTEX, kMinimalVert, "test.vert"},
                                   {duk::rhi::ShaderModule::FRAGMENT, kFragWithUBO, "ubo.frag"}});
        const auto& bufDesc = std::get<duk::rhi::BufferBindingDescription>(source.binding_layout()[0].binding);
        CHECK(bufDesc.size > 0);
        CHECK(bufDesc.stride >= bufDesc.size);
    }
}

TEST_CASE("RuntimeShaderDataSource storage buffer binding", "[rhi][runtime_shader_data_source]") {
    SECTION("SSBO in compute stage") {
        auto source = make_source({{duk::rhi::ShaderModule::COMPUTE, kComputeWithSSBO, "test.comp"}});
        const auto& layout = source.binding_layout();
        REQUIRE(layout.size() == 1);

        const auto& desc = layout[0];
        CHECK(desc.name == "data");
        CHECK(desc.moduleMask & duk::rhi::ShaderModule::COMPUTE);

        REQUIRE(std::holds_alternative<duk::rhi::BufferBindingDescription>(desc.binding));
        const auto& bufDesc = std::get<duk::rhi::BufferBindingDescription>(desc.binding);
        CHECK(bufDesc.type == duk::rhi::BufferBindingType::STORAGE_BUFFER);
    }
}

// ---------------------------------------------------------------------------
// Binding layout — cross-stage sharing
// ---------------------------------------------------------------------------

TEST_CASE("RuntimeShaderDataSource shared bindings", "[rhi][runtime_shader_data_source]") {
    SECTION("same binding in vertex and fragment merges into one entry with combined mask") {
        auto source = make_source({{duk::rhi::ShaderModule::VERTEX, kVertWithSharedUBO, "shared.vert"},
                                   {duk::rhi::ShaderModule::FRAGMENT, kFragWithSharedUBO, "shared.frag"}});
        const auto& layout = source.binding_layout();
        REQUIRE(layout.size() == 1);
        CHECK(layout[0].name == "camera");
        CHECK(layout[0].moduleMask & duk::rhi::ShaderModule::VERTEX);
        CHECK(layout[0].moduleMask & duk::rhi::ShaderModule::FRAGMENT);
    }

    SECTION("multiple bindings are all present with correct types") {
        auto source = make_source({{duk::rhi::ShaderModule::VERTEX, kVertWithTextureAndUBO, "multi.vert"},
                                   {duk::rhi::ShaderModule::FRAGMENT, kFragWithTextureAndUBO, "multi.frag"}});
        const auto& layout = source.binding_layout();
        REQUIRE(layout.size() == 2);

        const auto albedoIdx = source.binding_index("albedo");
        const auto materialIdx = source.binding_index("material");

        CHECK(std::holds_alternative<duk::rhi::ImageBindingDescription>(layout[albedoIdx].binding));
        CHECK(std::holds_alternative<duk::rhi::BufferBindingDescription>(layout[materialIdx].binding));
    }
}

// ---------------------------------------------------------------------------
// binding_index lookup
// ---------------------------------------------------------------------------

TEST_CASE("RuntimeShaderDataSource binding_index", "[rhi][runtime_shader_data_source]") {
    SECTION("resolved index points to the correct binding") {
        auto source = make_source({{duk::rhi::ShaderModule::VERTEX, kVertWithTextureAndUBO, "multi.vert"},
                                   {duk::rhi::ShaderModule::FRAGMENT, kFragWithTextureAndUBO, "multi.frag"}});
        const auto& layout = source.binding_layout();

        CHECK(layout[source.binding_index("albedo")].name == "albedo");
        CHECK(layout[source.binding_index("material")].name == "material");
    }

    SECTION("unknown name throws std::out_of_range") {
        auto source = make_source({{duk::rhi::ShaderModule::VERTEX, kMinimalVert, "test.vert"},
                                   {duk::rhi::ShaderModule::FRAGMENT, kMinimalFrag, "test.frag"}});
        CHECK_THROWS_AS(source.binding_index("nonExistent"), std::out_of_range);
    }
}

// ---------------------------------------------------------------------------
// Hash
// ---------------------------------------------------------------------------

TEST_CASE("RuntimeShaderDataSource hash", "[rhi][runtime_shader_data_source]") {
    SECTION("same GLSL source produces the same hash") {
        auto a = make_source({{duk::rhi::ShaderModule::VERTEX, kMinimalVert, "test.vert"},
                              {duk::rhi::ShaderModule::FRAGMENT, kMinimalFrag, "test.frag"}});
        auto b = make_source({{duk::rhi::ShaderModule::VERTEX, kMinimalVert, "test.vert"},
                              {duk::rhi::ShaderModule::FRAGMENT, kMinimalFrag, "test.frag"}});
        CHECK(a.hash() == b.hash());
    }

    SECTION("different GLSL source produces a different hash") {
        auto a = make_source({{duk::rhi::ShaderModule::VERTEX, kMinimalVert, "test.vert"},
                              {duk::rhi::ShaderModule::FRAGMENT, kMinimalFrag, "test.frag"}});
        auto b = make_source({{duk::rhi::ShaderModule::VERTEX, kMinimalVertAlt, "alt.vert"},
                              {duk::rhi::ShaderModule::FRAGMENT, kMinimalFrag, "test.frag"}});
        CHECK(a.hash() != b.hash());
    }

    SECTION("hash is non-zero for a non-trivial shader") {
        auto source = make_source({{duk::rhi::ShaderModule::VERTEX, kVertWithAttributes, "attrs.vert"},
                                   {duk::rhi::ShaderModule::FRAGMENT, kFragWithSampler, "sampler.frag"}});
        CHECK(source.hash() != 0);
    }
}


