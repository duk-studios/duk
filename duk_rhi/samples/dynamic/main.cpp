//
// duk_rhi sample: dynamic
//
// Demonstrates:
//   - A single BufferAllocator shared by every descriptor binding across
//     multiple draw calls in the same frame.
//   - Two quads, each with its own model matrix (MatricesUBO) and tint
//     colour (ColorUBO), both sub-allocated from that one buffer via
//     BufferAllocator::alloc() every frame.
//   - Because all buffer bindings use dynamic Vulkan offsets, the same
//     descriptor set is reused for both quads — only the offsets differ
//     at vkCmdBindDescriptorSets time.
//

#include <duk_log/logger.h>
#include <duk_log/log.h>
#include <duk_log/sinks/term_color_sink.h>

#include <duk_platform/platform.h>

#include <duk_rhi/instance.h>
#include <duk_rhi/buffer_allocator.h>
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

uniform MatricesUBO {
    mat4 model;
    mat4 view;
    mat4 proj;
} matrices;

void main() {
    gl_Position = matrices.proj * matrices.view * matrices.model * vec4(inPosition, 1.0);
}
)glsl";

static const char* kFragmentGlsl = R"glsl(
#version 450

layout(location = 0) out vec4 outColor;

uniform ColorUBO {
    vec4 color;
} tint;

void main() {
    outColor = tint.color;
}
)glsl";

// -----------------------------------------------------------------------
// Geometry — a single unit quad in the XY plane
// -----------------------------------------------------------------------

static const glm::vec3 kPositions[] = {
        {-0.5f, -0.5f, 0.0f},// 0 bottom-left
        {0.5f, -0.5f, 0.0f}, // 1 bottom-right
        {0.5f, 0.5f, 0.0f},  // 2 top-right
        {-0.5f, 0.5f, 0.0f}, // 3 top-left
};

static const uint16_t kIndices[] = {
        0, 1, 2, 2, 3, 0,
};

static constexpr uint32_t kIndexCount = static_cast<uint32_t>(std::size(kIndices));

// -----------------------------------------------------------------------
// Per-draw uniform structs (one alloc per struct per draw each frame)
// -----------------------------------------------------------------------

struct MatricesUBO {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

struct ColorUBO {
    glm::vec4 color;
};

// -----------------------------------------------------------------------
// Helper: allocate + upload one quad's uniforms; returns the offsets
// -----------------------------------------------------------------------

struct QuadAllocation {
    duk::rhi::BufferAllocation matrices;
    duk::rhi::BufferAllocation color;
};

static QuadAllocation upload_quad(duk::rhi::BufferAllocator& allocator, const MatricesUBO& matrices, const ColorUBO& color) {
    QuadAllocation a{};
    a.matrices = allocator.alloc(sizeof(MatricesUBO)).value();
    a.color = allocator.alloc(sizeof(ColorUBO)).value();
    allocator.buffer()->write(matrices, a.matrices.offset);
    allocator.buffer()->write(color, a.color.offset);
    return a;
}

// -----------------------------------------------------------------------
// Helper: draw one quad using previously uploaded offsets
// -----------------------------------------------------------------------

static void draw_quad(const duk::rhi::RenderCommands& render, const duk::rhi::Buffer& buffer, uint32_t matricesSlot, uint32_t colorSlot, const QuadAllocation& alloc) {
    duk::rhi::ShaderBindings resources{};
    resources.resources[matricesSlot] = duk::rhi::BufferResource{&buffer, static_cast<uint32_t>(alloc.matrices.offset)};
    resources.resources[colorSlot] = duk::rhi::BufferResource{&buffer, static_cast<uint32_t>(alloc.color.offset)};
    render.bind_resources(resources);

    duk::rhi::DrawIndexedParams drawParams;
    drawParams.indexCount = kIndexCount;
    drawParams.firstIndex = 0;
    drawParams.instanceCount = 1;
    drawParams.firstInstance = 0;
    drawParams.vertexOffset = 0;
    render.draw_indexed(drawParams);
}

// -----------------------------------------------------------------------
// Entry point
// -----------------------------------------------------------------------

int main() {
    // -------------------------------------------------------------------
    // Window
    // -------------------------------------------------------------------
    auto platform = duk::platform::create_platform();

    duk::log::instance()->add_sink(std::make_unique<duk::log::TermColorSink>("duk", duk::log::Level::INFO));

    duk::platform::WindowCreateInfo windowCreateInfo = {};
    windowCreateInfo.title = "duk_rhi - dynamic";
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

    // -------------------------------------------------------------------
    // RHI device
    // -------------------------------------------------------------------
    duk::rhi::InstanceCreateInfo rhiCreateInfo = {};
    rhiCreateInfo.applicationName = "dynamic_sample";
    rhiCreateInfo.applicationVersion = 1;
    rhiCreateInfo.engineName = "duk";
    rhiCreateInfo.engineVersion = 1;
    rhiCreateInfo.api = duk::rhi::API::VULKAN;
    rhiCreateInfo.validationLayers = true;
    rhiCreateInfo.deviceIndex = 0;
    rhiCreateInfo.logger = duk::log::instance()->default_logger();

    auto rhi = duk::rhi::Instance::create(rhiCreateInfo);

    // -------------------------------------------------------------------
    // Command context
    // -------------------------------------------------------------------
    duk::rhi::CommandContextCreateInfo contextCreateInfo = {};
    contextCreateInfo.type = duk::rhi::CommandQueue::Type::GRAPHICS;
    contextCreateInfo.window = window.get();
    contextCreateInfo.framesInFlight = 2;
    auto ctx = rhi->create_command_context(contextCreateInfo);

    // -------------------------------------------------------------------
    // Shader
    // -------------------------------------------------------------------
    duk::rhi::ShaderCompilerCreateInfo compilerCreateInfo{};
    compilerCreateInfo.api = rhiCreateInfo.api;
    duk::rhi::ShaderCompiler shaderCompiler(compilerCreateInfo);

    duk::rhi::RuntimeShaderDataSourceCreateInfo shaderSourceCreateInfo = {};
    shaderSourceCreateInfo.vertexShaderCode = shaderCompiler.compile(kVertexGlsl, duk::rhi::ShaderModule::VERTEX, "dynamic.vert").value();
    shaderSourceCreateInfo.fragmentShaderCode = shaderCompiler.compile(kFragmentGlsl, duk::rhi::ShaderModule::FRAGMENT, "dynamic.frag").value();
    auto shaderDataSource = duk::rhi::RuntimeShaderDataSource(shaderSourceCreateInfo);

    duk::rhi::ShaderCreateInfo shaderCreateInfo = {};
    shaderCreateInfo.shaderDataSource = &shaderDataSource;
    auto shader = ctx->create_shader(shaderCreateInfo);

    // -------------------------------------------------------------------
    // Geometry buffers — static, uploaded once
    // -------------------------------------------------------------------
    duk::rhi::BufferCreateInfo positionBufferInfo = {};
    positionBufferInfo.type = duk::rhi::Buffer::Type::VERTEX;
    positionBufferInfo.updateFrequency = duk::rhi::Buffer::UpdateFrequency::STATIC;
    positionBufferInfo.size = sizeof(kPositions);
    auto positionBuffer = ctx->create_buffer(positionBufferInfo);

    duk::rhi::BufferCreateInfo indexBufferInfo = {};
    indexBufferInfo.type = duk::rhi::Buffer::Type::INDEX;
    indexBufferInfo.updateFrequency = duk::rhi::Buffer::UpdateFrequency::STATIC;
    indexBufferInfo.size = sizeof(kIndices);
    auto indexBuffer = ctx->create_buffer(indexBufferInfo);

    ctx->prepare();
    {
        auto transfer = ctx->transfer();
        transfer.copy_to_buffer(positionBuffer.get(), 0, sizeof(kPositions), kPositions);
        transfer.copy_to_buffer(indexBuffer.get(), 0, sizeof(kIndices), kIndices);
    }
    ctx->submit();

    // -------------------------------------------------------------------
    // Single BufferAllocator shared by all uniform bindings every frame.
    //
    // create_dynamic_buffer handles alignment rounding and buffer mapping
    // internally. bufferInfo.size is the desired per-frame capacity;
    // the actual buffer will be roundUp(size, alignment) * framesInFlight.
    // -------------------------------------------------------------------
    constexpr uint32_t kFramesInFlight = 2;

    duk::rhi::BufferAllocatorCreateInfo bufferAllocatorInfo = {};
    bufferAllocatorInfo.type = duk::rhi::Buffer::Type::UNIFORM;
    bufferAllocatorInfo.updateFrequency = duk::rhi::Buffer::UpdateFrequency::DYNAMIC;
    bufferAllocatorInfo.size = 4096;
    bufferAllocatorInfo.alignment = rhi->capabilities().minUniformBufferOffsetAlignment;
    bufferAllocatorInfo.framesInFlight = kFramesInFlight;
    auto allocator = duk::rhi::BufferAllocator(*ctx, bufferAllocatorInfo);

    // -------------------------------------------------------------------
    // Per-frame state
    // -------------------------------------------------------------------
    duk::rhi::PipelineState pipelineState{};
    pipelineState.rasterizer.topology = duk::rhi::PipelineState::Rasterizer::Topology::TRIANGLE_LIST;
    pipelineState.rasterizer.cullMode = duk::rhi::PipelineState::Rasterizer::CullMode::NONE;

    const auto matricesSlot = shaderDataSource.binding_index("matrices");
    const auto colorSlot = shaderDataSource.binding_index("tint");

    auto startTime = std::chrono::steady_clock::now();

    // Shared view + projection (same camera for every draw this frame).
    glm::mat4 view{};
    glm::mat4 proj{};

    // -------------------------------------------------------------------
    // Main loop
    // -------------------------------------------------------------------
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

        view = glm::lookAt(glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        proj = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);

        ctx->prepare_present();

        // Reset the allocator at the start of every frame.
        allocator.reset();

        // -- Phase 1: alloc offsets and upload uniform data --
        //    alloc() returns an Allocation with the aligned byte offset.
        //    We then write the data explicitly via write_buffer.

        MatricesUBO quad0Matrices{};
        quad0Matrices.model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.1f, 0.0f, 0.0f));
        quad0Matrices.model = glm::rotate(quad0Matrices.model, elapsed * glm::radians(60.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        quad0Matrices.view = view;
        quad0Matrices.proj = proj;
        ColorUBO quad0Color{};
        quad0Color.color = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);// red
        const auto quad0Alloc = upload_quad(allocator, quad0Matrices, quad0Color);

        MatricesUBO quad1Matrices{};
        quad1Matrices.model = glm::translate(glm::mat4(1.0f), glm::vec3(1.1f, 0.0f, 0.0f));
        quad1Matrices.model = glm::rotate(quad1Matrices.model, -elapsed * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        quad1Matrices.view = view;
        quad1Matrices.proj = proj;
        ColorUBO quad1Color{};
        quad1Color.color = glm::vec4(0.2f, 0.4f, 1.0f, 1.0f);// blue
        const auto quad1Alloc = upload_quad(allocator, quad1Matrices, quad1Color);


        // -- render using the uploaded offsets --
        {
            duk::rhi::RenderBeginParams renderBeginParams;
            renderBeginParams.clearColor = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
            renderBeginParams.loadOp = duk::rhi::LoadOp::CLEAR;
            renderBeginParams.storeOp = duk::rhi::StoreOp::STORE;

            auto render = ctx->render(renderBeginParams);

            // Shader, vertex buffer, and index buffer are shared by all quads.
            render.bind_shader(shader.get(), pipelineState);

            duk::rhi::ShaderInput input;
            input.vertex[0] = duk::rhi::BufferResource{positionBuffer.get(), 0};
            input.index = {duk::rhi::BufferResource{indexBuffer.get(), 0}, duk::rhi::IndexType::UINT16};
            render.bind_input(input);

            // Quad 0: left, red, rotates counter-clockwise
            draw_quad(render, *allocator.buffer(), matricesSlot, colorSlot, quad0Alloc);

            // Quad 1: right, blue, rotates clockwise
            draw_quad(render, *allocator.buffer(), matricesSlot, colorSlot, quad1Alloc);
        }

        ctx->submit();
    }

    return 0;
}
