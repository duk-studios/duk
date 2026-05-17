//
// duk_rhi sample: axes
//
// Draws one line per world axis:
//   X  →  red    ( -1,  0,  0 ) → ( 1,  0,  0 )
//   Y  →  green  (  0, -1,  0 ) → ( 0,  1,  0 )
//   Z  →  blue   (  0,  0, -1 ) → ( 0,  0,  1 )
//
// Coordinate convention: Y up, Z backwards (out of the screen), X to the right.
//
// Orbit camera controls:
//   Arrow keys  — orbit (yaw / pitch)
//   Q / E       — zoom in / out
//

#include <duk_log/logger.h>
#include <duk_log/log.h>
#include <duk_log/sinks/term_color_sink.h>

#include <duk_platform/platform.h>

#include <duk_rhi/instance.h>
#include <duk_rhi/runtime_shader_data_source.h>
#include <duk_rhi/pipeline_state.h>

#include <shaderc/shaderc.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <unordered_set>

// -----------------------------------------------------------------------
// GLSL shaders
// -----------------------------------------------------------------------

static const char* kVertexGlsl = R"glsl(
#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

uniform MatricesUBO {
    mat4 model;
    mat4 view;
    mat4 proj;
} matrices;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = matrices.proj * matrices.view * matrices.model * vec4(inPosition, 1.0);
    fragColor   = inColor;
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
// Axis geometry — 3 lines, 2 vertices each
// -----------------------------------------------------------------------

static const glm::vec3 kPositions[] = {
    // X axis
    {-1.0f,  0.0f,  0.0f},
    { 1.0f,  0.0f,  0.0f},
    // Y axis
    { 0.0f, -1.0f,  0.0f},
    { 0.0f,  1.0f,  0.0f},
    // Z axis
    { 0.0f,  0.0f, -1.0f},
    { 0.0f,  0.0f,  1.0f},
};

static const glm::vec3 kColors[] = {
    {0.25f, 0.0f,  0.0f},  // X negative — dim red
    {1.0f,  0.0f,  0.0f},  // X positive — bright red
    {0.0f,  0.25f, 0.0f},  // Y negative — dim green
    {0.0f,  1.0f,  0.0f},  // Y positive — bright green
    {0.0f,  0.0f,  0.25f}, // Z negative — dim blue
    {0.0f,  0.0f,  1.0f},  // Z positive — bright blue
};

static constexpr uint32_t kVertexCount = static_cast<uint32_t>(std::size(kPositions));

// -----------------------------------------------------------------------
// Per-frame uniform data
// -----------------------------------------------------------------------

struct MatricesUBO {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

// -----------------------------------------------------------------------
// GLSL → SPIR-V helper
// -----------------------------------------------------------------------

static std::vector<uint8_t> compile_glsl(const char* source, shaderc_shader_kind kind, const char* debugName) {
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);
    options.SetOptimizationLevel(shaderc_optimization_level_zero);
    options.SetAutoBindUniforms(true);
    options.SetAutoMapLocations(true);

    auto result = compiler.CompileGlslToSpv(source, kind, debugName, options);
    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        duk::log::fatal("Shader compilation failed ({}): {}", debugName, result.GetErrorMessage());
    }

    const auto* byteBegin = reinterpret_cast<const uint8_t*>(result.cbegin());
    const auto* byteEnd   = reinterpret_cast<const uint8_t*>(result.cend());
    return {byteBegin, byteEnd};
}

int main() {

    // -----------------------------------------------------------------------
    // Window
    // -----------------------------------------------------------------------
    auto platform = duk::platform::create_platform();

    duk::log::instance()->add_sink(std::make_unique<duk::log::TermColorSink>("duk", duk::log::Level::INFO));

    duk::platform::WindowCreateInfo windowCreateInfo = {};
    windowCreateInfo.title  = "duk_rhi - axes";
    windowCreateInfo.width  = 1280;
    windowCreateInfo.height = 720;
    windowCreateInfo.style  = duk::platform::WindowStyle::STANDARD;

    auto window = platform->create_window(windowCreateInfo);

    volatile bool running = true;

    std::unordered_set<duk::platform::Keys> heldKeys;

    duk::event::Listener listener;
    listener.listen(window->window_close_event,   [&window]  { window->close(); });
    listener.listen(window->window_destroy_event, [&running] { running = false; });
    listener.listen(window->key_event, [&](duk::platform::Keys key, duk::platform::KeyModifiers::Mask, duk::platform::KeyAction action) {
        if (key == duk::platform::Keys::ESC && action == duk::platform::KeyAction::PRESS) {
            window->close();
        }
        if (action == duk::platform::KeyAction::PRESS) {
            heldKeys.insert(key);
        } else {
            heldKeys.erase(key);
        }
    });

    // -----------------------------------------------------------------------
    // RHI device
    // -----------------------------------------------------------------------
    duk::rhi::InstanceCreateInfo rhiCreateInfo = {};
    rhiCreateInfo.applicationName    = "axes_sample";
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
    duk::rhi::RuntimeShaderDataSourceCreateInfo shaderSourceCreateInfo = {};
    shaderSourceCreateInfo.vertexShaderCode   = compile_glsl(kVertexGlsl,   shaderc_vertex_shader,   "axes.vert");
    shaderSourceCreateInfo.fragmentShaderCode = compile_glsl(kFragmentGlsl, shaderc_fragment_shader, "axes.frag");
    auto shaderDataSource = duk::rhi::RuntimeShaderDataSource(shaderSourceCreateInfo);

    duk::rhi::ShaderCreateInfo shaderCreateInfo = {};
    shaderCreateInfo.shaderDataSource = &shaderDataSource;
    auto shader = ctx->create_shader(shaderCreateInfo);

    // -----------------------------------------------------------------------
    // Geometry buffers — static, uploaded once
    // -----------------------------------------------------------------------
    duk::rhi::BufferCreateInfo positionBufferInfo = {};
    positionBufferInfo.type            = duk::rhi::Buffer::Type::VERTEX;
    positionBufferInfo.updateFrequency = duk::rhi::Buffer::UpdateFrequency::STATIC;
    positionBufferInfo.size            = sizeof(kPositions);
    auto positionBuffer = ctx->create_buffer(positionBufferInfo);

    duk::rhi::BufferCreateInfo colorBufferInfo = {};
    colorBufferInfo.type            = duk::rhi::Buffer::Type::VERTEX;
    colorBufferInfo.updateFrequency = duk::rhi::Buffer::UpdateFrequency::STATIC;
    colorBufferInfo.size            = sizeof(kColors);
    auto colorBuffer = ctx->create_buffer(colorBufferInfo);

    ctx->prepare();
    {
        auto transfer = ctx->transfer();
        transfer.write_buffer(positionBuffer.get(), kPositions, sizeof(kPositions), 0);
        transfer.write_buffer(colorBuffer.get(),    kColors,    sizeof(kColors),    0);
    }
    ctx->submit();

    // -----------------------------------------------------------------------
    // Matrices uniform buffer — dynamic, written every frame
    // -----------------------------------------------------------------------
    duk::rhi::BufferCreateInfo uniformBufferInfo = {};
    uniformBufferInfo.type            = duk::rhi::Buffer::Type::UNIFORM;
    uniformBufferInfo.updateFrequency = duk::rhi::Buffer::UpdateFrequency::DYNAMIC;
    uniformBufferInfo.size            = sizeof(MatricesUBO);
    auto matricesUBO = ctx->create_buffer(uniformBufferInfo);

    const auto matricesSlot = shaderDataSource.binding_index("matrices");

    // -----------------------------------------------------------------------
    // Pipeline state — line list topology, no culling
    // -----------------------------------------------------------------------
    duk::rhi::PipelineState pipelineState{};
    pipelineState.rasterizer.topology = duk::rhi::PipelineState::Rasterizer::Topology::LINE_LIST;
    pipelineState.rasterizer.cullMode = duk::rhi::PipelineState::Rasterizer::CullMode::NONE;
    pipelineState.rasterizer.origin   = duk::rhi::PipelineState::Rasterizer::Origin::BOTTOM_LEFT;

    // -----------------------------------------------------------------------
    // Orbit camera state
    // -----------------------------------------------------------------------
    float azimuth  =  glm::radians(0.0f);   // yaw around Y
    float elevation = glm::radians(20.0f);  // pitch above XZ plane
    float distance  = 4.0f;

    constexpr float kOrbitSpeed = glm::radians(60.0f);  // radians per second
    constexpr float kZoomSpeed  = 3.0f;                 // units per second
    constexpr float kMinDist    = 1.0f;
    constexpr float kMaxDist    = 20.0f;
    constexpr float kMaxElev    = glm::radians(89.0f);

    auto lastTime = std::chrono::steady_clock::now();

    // -----------------------------------------------------------------------
    // Main loop
    // -----------------------------------------------------------------------
    window->show();

    while (running) {
        platform->pool_events();

        if (window->minimized() || !window->valid()) {
            continue;
        }

        const auto  now = std::chrono::steady_clock::now();
        const float dt  = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        if (heldKeys.count(duk::platform::Keys::LEFT_ARROW))  azimuth   -= kOrbitSpeed * dt;
        if (heldKeys.count(duk::platform::Keys::RIGHT_ARROW)) azimuth   += kOrbitSpeed * dt;
        if (heldKeys.count(duk::platform::Keys::UP_ARROW))    elevation  = glm::min(elevation + kOrbitSpeed * dt, kMaxElev);
        if (heldKeys.count(duk::platform::Keys::DOWN_ARROW))  elevation  = glm::max(elevation - kOrbitSpeed * dt, -kMaxElev);
        if (heldKeys.count(duk::platform::Keys::Q))           distance   = glm::max(distance - kZoomSpeed * dt, kMinDist);
        if (heldKeys.count(duk::platform::Keys::E))           distance   = glm::min(distance + kZoomSpeed * dt, kMaxDist);

        const auto width  = window->width();
        const auto height = window->height();

        pipelineState.viewport.extent = {static_cast<float>(width), static_cast<float>(height)};
        pipelineState.scissor.extent  = {width, height};

        const glm::vec3 eye = {
            distance * glm::cos(elevation) * glm::sin(azimuth),
            distance * glm::sin(elevation),
            distance * glm::cos(elevation) * glm::cos(azimuth),
        };

        MatricesUBO ubo{};
        ubo.model = glm::mat4(1.0f);
        ubo.view  = glm::lookAt(eye, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ubo.proj  = glm::perspective(
            glm::radians(45.0f),
            static_cast<float>(width) / static_cast<float>(height),
            0.1f, 100.0f);

        ctx->prepare_present();

        {
            auto transfer = ctx->transfer();
            transfer.write_buffer(matricesUBO.get(), &ubo, sizeof(ubo), 0);
        }

        {
            duk::rhi::RenderBeginParams renderBeginParams;
            renderBeginParams.clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
            renderBeginParams.loadOp     = duk::rhi::LoadOp::CLEAR;
            renderBeginParams.storeOp    = duk::rhi::StoreOp::STORE;

            auto render = ctx->render(renderBeginParams);

            render.bind_shader(shader.get(), pipelineState);

            duk::rhi::ShaderResources resources{};
            resources.bindings[matricesSlot] = duk::rhi::BufferBinding{matricesUBO.get()};
            render.bind_resources(resources);

            const duk::rhi::Buffer* vertexBuffers[] = {positionBuffer.get(), colorBuffer.get()};
            render.bind_vertex_buffers(vertexBuffers, 2);

            duk::rhi::DrawParams drawParams;
            drawParams.vertexCount   = kVertexCount;
            drawParams.firstVertex   = 0;
            drawParams.instanceCount = 1;
            drawParams.firstInstance = 0;
            render.draw(drawParams);
        }

        ctx->submit();
    }

    return 0;
}

