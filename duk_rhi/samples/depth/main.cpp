//
// duk_rhi sample: depth
//
// Demonstrates depth testing with the default framebuffer depth attachment.
// Two cubes orbit a common centre 180 degrees apart, so they continuously
// pass in front of one another.  Without depth testing you would see
// draw-order artefacts; with depth testing the correct cube occludes the
// other regardless of submission order.
//

#include <duk_log/logger.h>
#include <duk_log/log.h>
#include <duk_log/sinks/term_color_sink.h>

#include <duk_platform/platform.h>

#include <duk_rhi/instance.h>
#include <duk_rhi/runtime_shader_data_source.h>
#include <duk_rhi/pipeline_state.h>
#include <duk_rhi/index_types.h>
#include <duk_rhi/shader_compiler.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

// -----------------------------------------------------------------------
// GLSL shaders
// -----------------------------------------------------------------------

static const char* kVertexGlsl = R"glsl(
#version 450

layout(location = 0) in vec3 inPosition;

uniform CameraUBO {
    mat4 view;
    mat4 proj;
} camera;

uniform ObjectUBO {
    mat4 model;
    vec4 color;
} object;

layout(location = 0) out vec4 fragColor;

void main() {
    gl_Position = camera.proj * camera.view * object.model * vec4(inPosition, 1.0);
    fragColor   = object.color;
}
)glsl";

static const char* kFragmentGlsl = R"glsl(
#version 450

layout(location = 0) in  vec4 fragColor;
layout(location = 0) out vec4 outColor;

void main() {
    outColor = fragColor;
}
)glsl";

// -----------------------------------------------------------------------
// Geometry — unit cube (positions only)
// -----------------------------------------------------------------------

static const glm::vec3 kPositions[] = {
        {-0.5f, -0.5f, -0.5f},// 0
        {0.5f, -0.5f, -0.5f}, // 1
        {0.5f, 0.5f, -0.5f},  // 2
        {-0.5f, 0.5f, -0.5f}, // 3
        {-0.5f, -0.5f, 0.5f}, // 4
        {0.5f, -0.5f, 0.5f},  // 5
        {0.5f, 0.5f, 0.5f},   // 6
        {-0.5f, 0.5f, 0.5f},  // 7
};

static const uint16_t kIndices[] = {
        0, 1, 2, 2, 3, 0,// front
        1, 5, 6, 6, 2, 1,// right
        5, 4, 7, 7, 6, 5,// back
        4, 0, 3, 3, 7, 4,// left
        3, 2, 6, 6, 7, 3,// top
        4, 5, 1, 1, 0, 4,// bottom
};

static constexpr uint32_t kIndexCount = static_cast<uint32_t>(std::size(kIndices));

// -----------------------------------------------------------------------
// Per-frame uniform data
// -----------------------------------------------------------------------

struct CameraUBO {
    glm::mat4 view;
    glm::mat4 proj;
};

struct ObjectUBO {
    glm::mat4 model;
    glm::vec4 color;
};

// -----------------------------------------------------------------------
// Helper — build a model matrix for an orbiting cube
//   angle       : current orbit angle in radians
//   orbitRadius : distance from the world origin
//   selfSpin    : additional rotation around Y applied to the cube itself
// -----------------------------------------------------------------------
static glm::mat4 orbit_model(float angle, float orbitRadius, float selfSpin) {
    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, glm::vec3(glm::sin(angle) * orbitRadius, 0.0f, glm::cos(angle) * orbitRadius));
    m = glm::rotate(m, selfSpin, glm::vec3(0.0f, 1.0f, 0.0f));
    m = glm::rotate(m, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    return m;
}

int main() {
    // -----------------------------------------------------------------------
    // Window
    // -----------------------------------------------------------------------
    auto platform = duk::platform::create_platform();

    duk::log::instance()->add_sink(std::make_unique<duk::log::TermColorSink>("duk", duk::log::Level::INFO));

    duk::platform::WindowCreateInfo windowCreateInfo = {};
    windowCreateInfo.title = "duk_rhi - depth";
    windowCreateInfo.width = 1280;
    windowCreateInfo.height = 720;
    windowCreateInfo.style = duk::platform::WindowStyle::STANDARD;

    auto window = platform->create_window(windowCreateInfo);

    volatile bool running = true;

    duk::event::Listener listener;
    listener.listen(window->window_close_event, [&window] {
        window->close();
    });
    listener.listen(window->window_destroy_event, [&running] {
        running = false;
    });
    listener.listen(window->key_event, [&window](duk::platform::Keys key, duk::platform::KeyModifiers::Mask, duk::platform::KeyAction action) {
        if (key == duk::platform::Keys::ESC && action == duk::platform::KeyAction::PRESS) {
            window->close();
        }
    });

    // -----------------------------------------------------------------------
    // RHI device
    // -----------------------------------------------------------------------
    duk::rhi::InstanceCreateInfo rhiCreateInfo = {};
    rhiCreateInfo.applicationName = "depth_sample";
    rhiCreateInfo.applicationVersion = 1;
    rhiCreateInfo.engineName = "duk";
    rhiCreateInfo.engineVersion = 1;
    rhiCreateInfo.api = duk::rhi::API::VULKAN;
    rhiCreateInfo.validationLayers = true;
    rhiCreateInfo.deviceIndex = 0;
    rhiCreateInfo.logger = duk::log::instance()->default_logger();

    auto rhi = duk::rhi::Instance::create(rhiCreateInfo);

    // -----------------------------------------------------------------------
    // Command context — depth enabled via D32_SFLOAT
    // -----------------------------------------------------------------------
    duk::rhi::CommandContextCreateInfo contextCreateInfo = {};
    contextCreateInfo.type = duk::rhi::CommandQueue::Type::GRAPHICS;
    contextCreateInfo.window = window.get();
    contextCreateInfo.framesInFlight = 2;
    contextCreateInfo.depthFormat = duk::rhi::PixelFormat::D32_SFLOAT;
    auto ctx = rhi->create_command_context(contextCreateInfo);

    // -----------------------------------------------------------------------
    // Shader
    // -----------------------------------------------------------------------
    duk::rhi::ShaderCompilerCreateInfo compilerCreateInfo{};
    compilerCreateInfo.api = rhiCreateInfo.api;
    duk::rhi::ShaderCompiler shaderCompiler(compilerCreateInfo);

    duk::rhi::RuntimeShaderDataSourceCreateInfo shaderSourceCreateInfo = {};
    shaderSourceCreateInfo.vertexShaderCode = shaderCompiler.compile(kVertexGlsl, duk::rhi::ShaderModule::VERTEX, "depth.vert").value();
    shaderSourceCreateInfo.fragmentShaderCode = shaderCompiler.compile(kFragmentGlsl, duk::rhi::ShaderModule::FRAGMENT, "depth.frag").value();
    auto shaderDataSource = duk::rhi::RuntimeShaderDataSource(shaderSourceCreateInfo);

    duk::rhi::ShaderCreateInfo shaderCreateInfo = {};
    shaderCreateInfo.shaderDataSource = &shaderDataSource;
    auto shader = ctx->create_shader(shaderCreateInfo);

    const auto cameraSlot = shaderDataSource.binding_index("camera");
    const auto objectSlot = shaderDataSource.binding_index("object");

    // -----------------------------------------------------------------------
    // Geometry buffers (shared by both cubes)
    // -----------------------------------------------------------------------
    duk::rhi::BufferCreateInfo positionBufferCreateInfo = {};
    positionBufferCreateInfo.type = duk::rhi::BufferType::VERTEX;
    positionBufferCreateInfo.properties = duk::rhi::BufferProperties::DEVICE_LOCAL;
    positionBufferCreateInfo.size = sizeof(kPositions);
    auto positionBuffer = ctx->create_buffer(positionBufferCreateInfo);

    duk::rhi::BufferCreateInfo indexBufferCreateInfo = {};
    indexBufferCreateInfo.type = duk::rhi::BufferType::INDEX;
    indexBufferCreateInfo.properties = duk::rhi::BufferProperties::DEVICE_LOCAL;
    indexBufferCreateInfo.size = sizeof(kIndices);
    auto indexBuffer = ctx->create_buffer(indexBufferCreateInfo);

    // -----------------------------------------------------------------------
    // Uniform buffers
    // -----------------------------------------------------------------------
    duk::rhi::BufferCreateInfo uniformBufferCreateInfo = {};
    uniformBufferCreateInfo.type = duk::rhi::BufferType::UNIFORM;
    uniformBufferCreateInfo.properties = duk::rhi::BufferProperties::HOST_COHERENT;
    uniformBufferCreateInfo.size = sizeof(CameraUBO);
    auto cameraUBOBuffer = ctx->create_buffer(uniformBufferCreateInfo);
    ctx->map_buffer(cameraUBOBuffer.get());

    uniformBufferCreateInfo.size = sizeof(ObjectUBO);
    auto objectUBOBufferA = ctx->create_buffer(uniformBufferCreateInfo);
    auto objectUBOBufferB = ctx->create_buffer(uniformBufferCreateInfo);
    ctx->map_buffer(objectUBOBufferA.get());
    ctx->map_buffer(objectUBOBufferB.get());

    // Upload static mesh data
    ctx->prepare();
    {
        auto transfer = ctx->transfer();
        transfer.copy_to_buffer(positionBuffer.get(), 0, sizeof(kPositions), kPositions);
        transfer.copy_to_buffer(indexBuffer.get(), 0, sizeof(kIndices), kIndices);
    }
    ctx->submit();

    // -----------------------------------------------------------------------
    // Pipeline state — depth testing on, back-face culling, bottom-left origin
    // -----------------------------------------------------------------------
    duk::rhi::PipelineState pipelineState{};
    pipelineState.rasterizer.topology = duk::rhi::PipelineState::Rasterizer::Topology::TRIANGLE_LIST;
    pipelineState.rasterizer.cullMode = duk::rhi::PipelineState::Rasterizer::CullMode::BACK;
    pipelineState.rasterizer.origin = duk::rhi::PipelineState::Rasterizer::Origin::BOTTOM_LEFT;
    pipelineState.rasterizer.depthTesting = true;

    auto startTime = std::chrono::steady_clock::now();

    // -----------------------------------------------------------------------
    // Main loop
    // -----------------------------------------------------------------------
    window->show();

    while (running) {
        platform->pool_events();

        if (window->minimized() || !window->valid()) {
            continue;
        }

        const auto now = std::chrono::steady_clock::now();
        const float elapsed = std::chrono::duration<float>(now - startTime).count();

        const auto width = window->width();
        const auto height = window->height();

        pipelineState.viewport.extent = {static_cast<float>(width), static_cast<float>(height)};
        pipelineState.scissor.extent = {width, height};

        const float orbitAngle = elapsed * glm::radians(60.0f);
        constexpr float kOrbitRadius = 1.5f;

        // Camera — fixed, slightly above and behind, looking at origin
        CameraUBO cameraUbo{};
        cameraUbo.view = glm::lookAt(glm::vec3(0.0f, 0.5f, 5.0f),
                                     glm::vec3(0.0f, 0.0f, 0.0f),
                                     glm::vec3(0.0f, 1.0f, 0.0f));
        cameraUbo.proj = glm::perspective(glm::radians(45.0f),
                                          static_cast<float>(width) / static_cast<float>(height),
                                          0.1f, 100.0f);

        // Cube A — orange, at angle t
        ObjectUBO objectUboA{};
        objectUboA.model = orbit_model(orbitAngle, kOrbitRadius, elapsed * glm::radians(80.0f));
        objectUboA.color = glm::vec4(1.0f, 0.45f, 0.1f, 1.0f);

        // Cube B — teal, opposite side (angle t + pi)
        ObjectUBO objectUboB{};
        objectUboB.model = orbit_model(orbitAngle + glm::pi<float>(), kOrbitRadius, elapsed * glm::radians(-60.0f));
        objectUboB.color = glm::vec4(0.1f, 0.7f, 1.0f, 1.0f);

        ctx->prepare_present();

        cameraUBOBuffer->write(cameraUbo);
        objectUBOBufferA->write(objectUboA);
        objectUBOBufferB->write(objectUboB);

        {
            duk::rhi::RenderBeginParams renderBeginParams;
            renderBeginParams.clearColor = glm::vec4(0.05f, 0.05f, 0.08f, 1.0f);
            renderBeginParams.loadOp = duk::rhi::LoadOp::CLEAR;
            renderBeginParams.storeOp = duk::rhi::StoreOp::STORE;

            auto render = ctx->render(renderBeginParams);

            duk::rhi::ShaderInput input = {};
            input.vertex[0] = {positionBuffer.get()};
            input.index.resource = {indexBuffer.get()};
            input.index.type = duk::rhi::IndexType::UINT16;
            render.bind_input(input);

            duk::rhi::DrawIndexedParams drawParams;
            drawParams.indexCount = kIndexCount;
            drawParams.instanceCount = 1;

            // Draw cube A
            duk::rhi::ShaderBindings resourcesA{};
            resourcesA.resources[cameraSlot] = duk::rhi::BufferResource{cameraUBOBuffer.get()};
            resourcesA.resources[objectSlot] = duk::rhi::BufferResource{objectUBOBufferA.get()};
            render.bind_shader(shader.get(), pipelineState);
            render.bind_resources(resourcesA);
            render.draw_indexed(drawParams);

            // Draw cube B
            duk::rhi::ShaderBindings resourcesB{};
            resourcesB.resources[cameraSlot] = duk::rhi::BufferResource{cameraUBOBuffer.get()};
            resourcesB.resources[objectSlot] = duk::rhi::BufferResource{objectUBOBufferB.get()};
            render.bind_shader(shader.get(), pipelineState);
            render.bind_resources(resourcesB);
            render.draw_indexed(drawParams);
        }

        ctx->submit();
    }

    return 0;
}

