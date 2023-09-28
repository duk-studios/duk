/// 03/06/2023
/// application.cpp

#include <duk_sample/application.h>
#include <duk_platform/window.h>
#include <duk_rhi/vertex_types.h>
#include <duk_renderer/components/mesh_painter.h>
#include <duk_renderer/components/transform.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>

#include <cmath>
#include <fstream>
#include <chrono>

namespace duk::sample {

namespace detail {

struct PixelRGBA {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

using PixelRGBAImageDataSource = duk::rhi::StdImageDataSource<PixelRGBA, duk::rhi::Image::PixelFormat::R8G8B8A8_UNORM>;

template<typename T>
T lerp(T from, T to, float percent) {
    return (from * (1 - percent)) + (to * percent);
}

PixelRGBAImageDataSource load_image() {
    PixelRGBAImageDataSource imageDataSource(64, 64);
    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 64; y++) {
            auto xPercent = (float)x / 63;
            auto yPercent = (float)x / 63;

            auto r = lerp<uint8_t>(0, 255, xPercent);
            auto g = lerp<uint8_t>(0, 255, yPercent);
            auto b = lerp<uint8_t>(0, 255, xPercent * yPercent);

            imageDataSource.write_pixel(x, y, {r, g, b, 255});
        }
    }
    imageDataSource.update_hash();
    return imageDataSource;
}

using DefaultMeshDataSource = duk::renderer::MeshDataSourceT<duk::rhi::Vertex3DColorUV, uint16_t>;

DefaultMeshDataSource quad_mesh_data_source() {
    DefaultMeshDataSource meshDataSource;

    std::array<DefaultMeshDataSource::VertexType, 4> vertices = {};
    vertices[0] = {{0.5f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}};
    vertices[1] = {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};
    vertices[2] = {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}};
    vertices[3] = {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}};

    meshDataSource.insert_vertices(vertices.begin(), vertices.end());

    std::array<DefaultMeshDataSource::IndexType, 6> indices = {0, 1, 2, 2, 1, 3};

    meshDataSource.insert_indices(indices.begin(), indices.end());

    meshDataSource.update_hash();

    return meshDataSource;
}

DefaultMeshDataSource cube_mesh_data_source() {
    DefaultMeshDataSource meshDataSource;

    std::array<DefaultMeshDataSource::VertexType, 24> vertices = {};

    // front
    vertices[0] = {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}};
    vertices[1] = {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};
    vertices[2] = {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}};
    vertices[3] = {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}};

    // back
    vertices[4] = {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}};
    vertices[5] = {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};
    vertices[6] = {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 1.0f}};
    vertices[7] = {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f}};

    // left
    vertices[8] = {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}};
    vertices[9] = {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};
    vertices[10] = {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}};
    vertices[11] = {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}};

    // right
    vertices[12] = {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}};
    vertices[13] = {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};
    vertices[14] = {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}};
    vertices[15] = {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}};

    // top
    vertices[16] = {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}};
    vertices[17] = {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};
    vertices[18] = {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}};
    vertices[19] = {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}};

    // bottom
    vertices[20] = {{0.5f, -0.5f, 0.5f}, {1.0f, 0.5f, 0.2f, 1.0f}, {0.0f, 0.0f}};
    vertices[21] = {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.5f, 0.2f, 1.0f}, {1.0f, 0.0f}};
    vertices[22] = {{0.5f, -0.5f, -0.5f}, {1.0f, 0.5f, 0.2f, 1.0f}, {0.0f, 1.0f}};
    vertices[23] = {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.5f, 0.2f, 1.0f}, {1.0f, 1.0f}};



    meshDataSource.insert_vertices(vertices.begin(), vertices.end());

    std::array<DefaultMeshDataSource::IndexType, 36> indices = {
            0, 1, 2, 2, 1, 3, // front
            4, 5, 6, 6, 5, 7, // back
            8, 9, 10, 10, 9, 11, // right
            12, 13, 14, 14, 13, 15, // left
            16, 17, 18, 18, 17, 19, // top
            20, 21, 22, 22, 21, 23 // bottom
    };

    meshDataSource.insert_indices(indices.begin(), indices.end());

    meshDataSource.update_hash();

    return meshDataSource;
}

}

std::vector<uint8_t> Application::load_bytes(const char* filepath) {
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);

    if (!file) {
        std::ostringstream oss;
        oss << "failed to open file at: " << filepath;
        throw std::runtime_error(oss.str());
    }

    std::vector<uint8_t> buffer(file.tellg());
    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(buffer.size()));

    return buffer;
}

Application::Application(const ApplicationCreateInfo& applicationCreateInfo) :
    m_logger(duk::log::DEBUG),
    m_run(false) {

    if (applicationCreateInfo.sink) {
        applicationCreateInfo.sink->flush_from(m_logger);
    }

    duk::platform::WindowCreateInfo windowCreateInfo = {};
    windowCreateInfo.windowTitle = applicationCreateInfo.name;
    windowCreateInfo.width = 1280;
    windowCreateInfo.height = 720;

    m_window = check_expected_result(duk::platform::Window::create_window(windowCreateInfo));
    m_listener.listen(m_window->window_close_event, [this] {
        m_window->close();
    });

    m_listener.listen(m_window->window_destroy_event, [this](){
        m_run = false;
    });

    m_listener.listen(m_window->window_resize_event, [this](uint32_t width, uint32_t height){
        if (m_window->minimized()) {
            return;
        }
        m_renderer->resize(width, height);
    });


    duk::renderer::ForwardRendererCreateInfo forwardRendererCreateInfo = {};
    forwardRendererCreateInfo.rendererCreateInfo.window = m_window.get();
    forwardRendererCreateInfo.rendererCreateInfo.logger = &m_logger;
    forwardRendererCreateInfo.rendererCreateInfo.api = rhi::API::VULKAN;

    m_renderer = std::make_unique<duk::renderer::ForwardRenderer>(forwardRendererCreateInfo);

    duk::renderer::MeshBufferPoolCreateInfo meshBufferPoolCreateInfo = {};
    meshBufferPoolCreateInfo.rhi = m_renderer->rhi();
    meshBufferPoolCreateInfo.commandQueue = m_renderer->main_command_queue();

    m_meshBufferPool = std::make_shared<renderer::MeshBufferPool>(meshBufferPoolCreateInfo);

    {
        auto quadDataSource = detail::quad_mesh_data_source();

        m_quadMesh = m_meshBufferPool->create_mesh(&quadDataSource);
    }
    {
        auto cubeDataSource = detail::cube_mesh_data_source();

        m_cubeMesh = m_meshBufferPool->create_mesh(&cubeDataSource);
    }


    duk::renderer::ColorPainterCreateInfo colorPainterCreateInfo = {};
    colorPainterCreateInfo.rhi = m_renderer->rhi();
    colorPainterCreateInfo.commandQueue = m_renderer->main_command_queue();

    m_colorPainter = std::make_shared<duk::renderer::ColorPainter>(colorPainterCreateInfo);

    m_colorPalette = m_colorPainter->make_palette();

    m_scene = std::make_shared<duk::scene::Scene>();

    auto lerp = [](auto a, auto b, float t) -> float {
        return (a * (1.0f - t)) + b * t;
    };

    const auto objCount = 50;
    const auto colCount = 10;
    const auto rowCount = objCount / std::min(colCount, objCount);

    for (int i = 0; i < objCount; i++) {

        const float colPercent = float(i % (colCount)) / (colCount - 1);
        const float rowPercent = std::floor((float)i / (colCount)) / (rowCount - 1);

        auto obj = m_scene->add_object();

        auto position3D = obj.add<duk::renderer::Position3D>();
        position3D->value = glm::vec3(lerp(-20, 20, colPercent), lerp(-20, 20, rowPercent), glm::linearRand(-50.f, 10.f));

        auto rotation3D = obj.add<duk::renderer::Rotation3D>();
        rotation3D->value = glm::radians(glm::linearRand(glm::vec3(-95.f), glm::vec3(95.f)));

        auto scale3D = obj.add<duk::renderer::Scale3D>();
        scale3D->value = glm::linearRand(glm::vec3(0.02f), glm::vec3(1.3f));

        auto meshPainter = obj.add<duk::renderer::MeshPainter>();
        meshPainter->mesh = glm::linearRand(-1, 2) > 0 ? m_cubeMesh.get() : m_quadMesh.get();
        meshPainter->painter = m_colorPainter.get();
        meshPainter->palette = m_colorPalette.get();


    }
}

Application::~Application() {

}

void Application::run() {

    using clock = std::chrono::high_resolution_clock;

    m_run = true;

    m_window->show();

    double time = 0;
    double deltaTime = 0;

    while (m_run) {
        auto start = clock::now();
        m_window->pool_events();

        update(time, deltaTime);

        if (!m_window->minimized()) {
            draw();
        }
        auto end = clock::now();
        deltaTime = std::chrono::duration<double>(end - start).count();
        time += deltaTime;
    }
}

void Application::update(double time, double deltaTime) {

//    for (auto object : m_scene->objects_with_components<duk::renderer::Position3D>()) {
//        auto pos = object.component<duk::renderer::Position3D>();
//        pos->value.x = std::sin((float)time) * 10;
//    }

}

void Application::draw() {
    m_colorPalette->clear();
    m_renderer->render(m_scene.get());
}

}

