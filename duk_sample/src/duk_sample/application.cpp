/// 03/06/2023
/// application.cpp

#include <duk_sample/application.h>
#include <duk_platform/window.h>
#include <duk_renderer/vertex_types.h>
#include <duk_renderer/components/mesh_drawing.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/components/camera.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>

#include <cmath>
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

using DefaultMeshDataSource = duk::renderer::MeshDataSourceT<duk::renderer::Vertex3DNormalUV, uint16_t>;

DefaultMeshDataSource quad_mesh_data_source() {
    DefaultMeshDataSource meshDataSource;

    std::array<DefaultMeshDataSource::VertexType, 4> vertices = {};
    vertices[0] = {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}};
    vertices[1] = {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};
    vertices[2] = {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}};
    vertices[3] = {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}};

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
    vertices[0] = {{0.5f, 0.5f, 0.5f},      {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}};
    vertices[1] = {{-0.5f, 0.5f, 0.5f},     {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};
    vertices[2] = {{0.5f, -0.5f, 0.5f},     {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}};
    vertices[3] = {{-0.5f, -0.5f, 0.5f},    {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}};

    // back
    vertices[4] = {{-0.5f, 0.5f, -0.5f},    {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}};
    vertices[5] = {{0.5f, 0.5f, -0.5f},     {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}};
    vertices[6] = {{-0.5f, -0.5f, -0.5f},   {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}};
    vertices[7] = {{0.5f, -0.5f, -0.5f},    {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}};

    // left
    vertices[8] = {{-0.5f, 0.5f, 0.5f},     {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}};
    vertices[9] = {{-0.5f, 0.5f, -0.5f},    {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}};
    vertices[10] = {{-0.5f, -0.5f, 0.5f},   {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}};
    vertices[11] = {{-0.5f, -0.5f, -0.5f},  {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}};

    // right
    vertices[12] = {{0.5f, 0.5f, -0.5f},    {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}};
    vertices[13] = {{0.5f, 0.5f, 0.5f},     {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}};
    vertices[14] = {{0.5f, -0.5f, -0.5f},   {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}};
    vertices[15] = {{0.5f, -0.5f, 0.5f},    {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}};

    // top
    vertices[16] = {{0.5f, 0.5f, -0.5f},    {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}};
    vertices[17] = {{-0.5f, 0.5f, -0.5f},   {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}};
    vertices[18] = {{0.5f, 0.5f, 0.5f},     {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}};
    vertices[19] = {{-0.5f, 0.5f, 0.5f},    {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}};

    // bottom
    vertices[20] = {{0.5f, -0.5f, 0.5f},    {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}};
    vertices[21] = {{-0.5f, -0.5f, 0.5f},   {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}};
    vertices[22] = {{0.5f, -0.5f, -0.5f},   {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}};
    vertices[23] = {{-0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}};



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

DefaultMeshDataSource sphere_mesh_data_source(uint32_t segments) {
    DefaultMeshDataSource meshDataSource;

    const auto rings = segments;

    std::vector<DefaultMeshDataSource::VertexType> vertices;

    for (int i = 0; i <= rings; ++i) {
        float phi = static_cast<float>(i) * glm::pi<float>() / static_cast<float>(rings);
        float sinPhi = std::sin(phi);
        float cosPhi = std::cos(phi);

        for (int j = 0; j <= segments; ++j) {
            float theta = static_cast<float>(j) * 2.0f * glm::pi<float>() / static_cast<float>(segments);
            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);

            float x = sinPhi * cosTheta;
            float y = cosPhi;
            float z = sinPhi * sinTheta;

            glm::vec3 position(x, y, z);
            glm::vec3 normal(x, y, z);
            glm::vec2 uv(static_cast<float>(j) / static_cast<float>(segments), static_cast<float>(i) / static_cast<float>(rings));

            vertices.push_back({position, normal, uv});
        }
    }

    meshDataSource.insert_vertices(vertices.begin(), vertices.end());

    std::vector<DefaultMeshDataSource::IndexType> indices;

    for (int i = 0; i < rings; ++i) {
        for (int j = 0; j < segments; ++j) {
            int nextRow = i + 1;
            int nextColumn = (j + 1) % segments;
            int currentIdx = i * (segments + 1) + j;
            int nextIdx = nextRow * (segments + 1) + j;
            int nextRightIdx = nextRow * (segments + 1) + nextColumn;

            // Front face (clockwise order)
            indices.push_back(currentIdx);
            indices.push_back(nextRightIdx);
            indices.push_back(nextIdx);

            // Back face (clockwise order)
            indices.push_back(currentIdx);
            indices.push_back(currentIdx + 1);
            indices.push_back(nextRightIdx);
        }
    }

    meshDataSource.insert_indices(indices.begin(), indices.end());

    meshDataSource.update_hash();

    return meshDataSource;
}

static void update_perspective(duk::scene::Component<duk::renderer::PerspectiveCamera> perspectiveCamera, uint32_t width, uint32_t height)
{
    perspectiveCamera->zNear = 0.1f;
    perspectiveCamera->zFar = 1000.f;
    perspectiveCamera->aspectRatio = (float)width / (float)height;
    perspectiveCamera->fovDegrees = 45.0f;
}

static void populate_scene(duk::scene::Scene* scene,
                           uint32_t objCount,
                           duk::renderer::Painter* painter,
                           duk::renderer::Palette* palette,
                           duk::renderer::Mesh* mesh
                           ) {

    const auto colCount = static_cast<uint32_t>(sqrt(static_cast<double>(objCount)));
    const auto rowCount = objCount / std::min(colCount, objCount);

    for (int i = 0; i < objCount; i++) {

        const float colPercent = float(i % (colCount)) / float(colCount - 1);
        const float rowPercent = std::floor((float)i / float(colCount)) / float(rowCount - 1);

        auto obj = scene->add_object();

        auto position3D = obj.add<duk::renderer::Position3D>();
        position3D->value = glm::vec3(lerp(-20, 20, colPercent), lerp(-20, 20, rowPercent), glm::linearRand(-50.f, 30.f));

        auto rotation3D = obj.add<duk::renderer::Rotation3D>();
        rotation3D->value = glm::radians(glm::linearRand(glm::vec3(-95.f), glm::vec3(95.f)));

        auto scale3D = obj.add<duk::renderer::Scale3D>();
        scale3D->value = glm::linearRand(glm::vec3(0.02f), glm::vec3(1.3f));

        auto meshDrawing = obj.add<duk::renderer::MeshDrawing>();
        meshDrawing->mesh = mesh;
        meshDrawing->painter = painter;
        meshDrawing->palette = palette;

    }
}

struct PivotRotator {
    glm::vec3 pivot;
    float distanceFromPivot;
    float angularSpeed;
};

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

    m_listener.listen(m_window->window_resize_event, [this] (uint32_t width, uint32_t height) {
        detail::update_perspective(m_camera.component<duk::renderer::PerspectiveCamera>(), width, height);
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

    {
        auto sphereDataSource = detail::sphere_mesh_data_source(20);

        m_sphereMesh = m_meshBufferPool->create_mesh(&sphereDataSource);
    }


    {
        duk::renderer::ColorPainterCreateInfo colorPainterCreateInfo = {};
        colorPainterCreateInfo.rhi = m_renderer->rhi();
        colorPainterCreateInfo.commandQueue = m_renderer->main_command_queue();

        m_colorPainter = std::make_shared<duk::renderer::ColorPainter>(colorPainterCreateInfo);

        m_colorPalette = m_colorPainter->make_palette();
    }

    {
        duk::renderer::PhongPainterCreateInfo phongPainterCreateInfo = {};
        phongPainterCreateInfo.rhi = m_renderer->rhi();
        phongPainterCreateInfo.commandQueue = m_renderer->main_command_queue();

        m_phongPainter = std::make_shared<duk::renderer::PhongPainter>(phongPainterCreateInfo);

        m_phongPalette = m_phongPainter->make_palette();
    }


    m_scene = std::make_shared<duk::scene::Scene>();

    detail::populate_scene(m_scene.get(), 250, m_phongPainter.get(), m_phongPalette.get(), m_cubeMesh.get());
    detail::populate_scene(m_scene.get(), 250, m_phongPainter.get(), m_phongPalette.get(), m_quadMesh.get());

    detail::populate_scene(m_scene.get(), 250, m_phongPainter.get(), m_phongPalette.get(), m_sphereMesh.get());
    detail::populate_scene(m_scene.get(), 250, m_phongPainter.get(), m_phongPalette.get(), m_cubeMesh.get());

//    detail::populate_scene(m_scene.get(), 250, m_colorPainter.get(), m_colorPalette.get(), m_cubeMesh.get());
//    detail::populate_scene(m_scene.get(), 250, m_colorPainter.get(), m_colorPalette.get(), m_quadMesh.get());

    {
        m_camera = m_scene->add_object();

        detail::update_perspective(m_camera.add<duk::renderer::PerspectiveCamera>(), m_renderer->render_width(), m_renderer->render_height());

        m_camera.add<duk::renderer::Position3D>();
        m_camera.add<duk::renderer::Rotation3D>();

        auto pivotRotator = m_camera.add<detail::PivotRotator>();
        pivotRotator->pivot = glm::vec3(0);
        pivotRotator->distanceFromPivot = 50.f;
        pivotRotator->angularSpeed = 30.f;

        m_renderer->use_as_camera(m_camera);
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

    auto [position, rotation, pivotRotator] = m_camera.components<duk::renderer::Position3D, duk::renderer::Rotation3D, detail::PivotRotator>();

    const auto speed = glm::radians(pivotRotator->angularSpeed);

    glm::vec3 offset = {
            cosf((float)time * speed) * pivotRotator->distanceFromPivot,
            5,
            sinf((float)time * speed) * pivotRotator->distanceFromPivot
    };

    position->value = pivotRotator->pivot + offset;

    // calculate rotation
    {
        const auto direction = -glm::normalize(offset);

        rotation->value = glm::quatLookAt(direction, glm::vec3(0, 1, 0));
    }
}

void Application::draw() {
    m_renderer->render(m_scene.get());
}

}

