//
// duk_rhi sample: triangle
//
// Demonstrates the minimal draw call using the RHI API:
//   RHI (device) + Surface (window) + CommandContext + CommandQueue
//   + a graphics Shader with hardcoded GLSL source compiled to SPIR-V at runtime.
//
// A single RGB triangle is rendered each frame with no vertex buffers,
// no descriptors and no push constants — just gl_VertexIndex in the vertex shader.
//

#include <duk_log/logger.h>
#include <duk_log/log.h>
#include <duk_log/sinks/term_color_sink.h>

#include <duk_platform/platform.h>

#include <duk_rhi/instance.h>
#include <duk_rhi/runtime_shader_data_source.h>
#include <duk_rhi/pipeline_state.h>

#include <shaderc/shaderc.hpp>

// -----------------------------------------------------------------------
// Hardcoded GLSL shaders
// -----------------------------------------------------------------------

static const char* kVertexGlsl = R"glsl(
#version 450

// Hardcoded clip-space positions for the three vertices.
vec2 positions[3] = vec2[](
    vec2( 0.0, -0.5),
    vec2( 0.5,  0.5),
    vec2(-0.5,  0.5)
);

// Per-vertex colours matched to the classic OpenGL/Vulkan tutorial triangle.
vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = colors[gl_VertexIndex];
}
)glsl";

static const char* kFragmentGlsl = R"glsl(
#version 450

layout(location = 0) in  vec3 fragColor;
layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1.0);
}
)glsl";

// -----------------------------------------------------------------------
// Helper: compile GLSL source to a packed SPIR-V byte vector
// -----------------------------------------------------------------------

static std::vector<uint8_t> compile_glsl(const char* source, shaderc_shader_kind kind, const char* debugName) {
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);
    options.SetOptimizationLevel(shaderc_optimization_level_zero);

    auto result = compiler.CompileGlslToSpv(source, kind, debugName, options);
    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        duk::log::fatal("Shader compilation failed ({}): {}", debugName, result.GetErrorMessage());
    }

    // result iterates over uint32_t SPIR-V words; reinterpret as bytes for StdShaderDataSource
    const uint32_t* wordBegin = result.cbegin();
    const uint32_t* wordEnd   = result.cend();
    const auto* byteBegin = reinterpret_cast<const uint8_t*>(wordBegin);
    const auto* byteEnd   = reinterpret_cast<const uint8_t*>(wordEnd);

    return {byteBegin, byteEnd};
}

int main() {

    // -----------------------------------------------------------------------
    // Window
    // -----------------------------------------------------------------------
    auto platform = duk::platform::create_platform();

    duk::log::instance()->add_sink(std::make_unique<duk::log::TermColorSink>("duk", duk::log::Level::INFO));

    duk::platform::WindowCreateInfo windowCreateInfo = {};
    windowCreateInfo.title  = "duk_rhi - triangle";
    windowCreateInfo.width  = 1280;
    windowCreateInfo.height = 720;
    windowCreateInfo.style  = duk::platform::WindowStyle::STANDARD;

    auto window = platform->create_window(windowCreateInfo);

    volatile bool running = true;

    duk::event::Listener listener;
    listener.listen(window->window_close_event,   [&window]          { window->close(); });
    listener.listen(window->window_destroy_event, [&running]         { running = false; });
    listener.listen(window->key_event, [&window](duk::platform::Keys key, duk::platform::KeyModifiers::Mask, duk::platform::KeyAction action) {
        if (key == duk::platform::Keys::ESC && action == duk::platform::KeyAction::PRESS) {
            window->close();
        }
    });

    // -----------------------------------------------------------------------
    // RHI device
    // -----------------------------------------------------------------------
    duk::rhi::InstanceCreateInfo rhiCreateInfo = {};
    rhiCreateInfo.applicationName    = "triangle_sample";
    rhiCreateInfo.applicationVersion = 1;
    rhiCreateInfo.engineName         = "duk";
    rhiCreateInfo.engineVersion      = 1;
    rhiCreateInfo.api                = duk::rhi::API::VULKAN;
    rhiCreateInfo.validationLayers   = true;
    rhiCreateInfo.deviceIndex        = 0;
    rhiCreateInfo.logger             = duk::log::instance()->default_logger();

    auto rhi = duk::rhi::Instance::create(rhiCreateInfo);

    // -----------------------------------------------------------------------
    // Command context
    // -----------------------------------------------------------------------
    duk::rhi::CommandContextCreateInfo contextCreateInfo = {};
    contextCreateInfo.type           = duk::rhi::CommandQueue::Type::GRAPHICS;
    contextCreateInfo.window         = window.get();
    contextCreateInfo.framesInFlight = 2;
    auto ctx = rhi->create_command_context(contextCreateInfo);

    // -----------------------------------------------------------------------
    // Shader
    // -----------------------------------------------------------------------
    auto shaderDataSource = std::make_shared<duk::rhi::RuntimeShaderDataSource>();
    shaderDataSource->insert_spir_v_code(
        duk::rhi::ShaderModule::VERTEX,
        compile_glsl(kVertexGlsl, shaderc_vertex_shader, "triangle.vert"));
    shaderDataSource->insert_spir_v_code(
        duk::rhi::ShaderModule::FRAGMENT,
        compile_glsl(kFragmentGlsl, shaderc_fragment_shader, "triangle.frag"));
    shaderDataSource->update_hash();

    duk::rhi::ShaderCreateInfo shaderCreateInfo = {};
    shaderCreateInfo.shaderDataSource = shaderDataSource.get();

    auto shader = ctx->create_shader(shaderCreateInfo);

    // -----------------------------------------------------------------------
    // Main loop
    // -----------------------------------------------------------------------
    window->show();

    // Configure pipeline state for this pass
    duk::rhi::PipelineState pipelineState{};

    while (running) {
        platform->pool_events();

        if (window->minimized() || !window->valid()) {
            continue;
        }

        ctx->prepare();

        const auto width = window->width();
        const auto height = window->height();

        pipelineState.viewport.extent = {width, height};
        pipelineState.scissor.extent  = {width, height};

        {
            duk::rhi::RenderBeginParams renderBeginParams;
            renderBeginParams.clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);

            auto render = ctx->render(renderBeginParams);

            duk::rhi::BindShaderParams bindShaderParams;
            bindShaderParams.shader    = shader.get();
            bindShaderParams.resources = nullptr;

            render.bind_shader(bindShaderParams, pipelineState);

            duk::rhi::DrawParams renderParams;
            renderParams.vertexCount   = 3;
            renderParams.firstVertex   = 0;
            renderParams.instanceCount = 1;
            renderParams.firstInstance = 0;

            render.draw(renderParams);
        }

        ctx->flush();
    }

    return 0;
}

