/// 03/06/2023
/// application.cpp

#include <duk_sample/application.h>
#include <duk_platform/window.h>
#include <duk_renderer/vertex_types.h>
#include <duk_renderer/materials/painter.h>
#include <duk_renderer/components/mesh_drawing.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/components/camera.h>
#include <duk_renderer/components/lighting.h>
#include <duk_import/importer.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>

#include <cmath>
#include <chrono>

namespace duk::sample {

namespace detail {

template<typename T>
T lerp(T from, T to, float percent) {
    return (from * (1 - percent)) + (to * percent);
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
    vertices[0] = {{ 0.5f,  0.5f, -0.5f},   {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}};
    vertices[1] = {{-0.5f,  0.5f, -0.5f},   {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}};
    vertices[2] = {{ 0.5f, -0.5f, -0.5f},   {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}};
    vertices[3] = {{-0.5f, -0.5f, -0.5f},   {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}};

    // back
    vertices[4] = {{-0.5f,  0.5f, 0.5f},    {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}};
    vertices[5] = {{ 0.5f,  0.5f, 0.5f},    {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};
    vertices[6] = {{-0.5f, -0.5f, 0.5f},    {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}};
    vertices[7] = {{ 0.5f, -0.5f, 0.5f},    {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}};

    // left
    vertices[8] =  {{-0.5f,  0.5f, -0.5f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}};
    vertices[9] =  {{-0.5f,  0.5f,  0.5f},  {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}};
    vertices[10] = {{-0.5f, -0.5f, -0.5f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}};
    vertices[11] = {{-0.5f, -0.5f,  0.5f},  {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}};

    // right
    vertices[12] = {{0.5f,  0.5f,  0.5f},   {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}};
    vertices[13] = {{0.5f,  0.5f, -0.5f},   {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}};
    vertices[14] = {{0.5f, -0.5f,  0.5f},   {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}};
    vertices[15] = {{0.5f, -0.5f, -0.5f},   {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}};

    // top
    vertices[16] = {{ 0.5f, 0.5f,  0.5f},   {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}};
    vertices[17] = {{-0.5f, 0.5f,  0.5f},   {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}};
    vertices[18] = {{ 0.5f, 0.5f, -0.5f},   {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}};
    vertices[19] = {{-0.5f, 0.5f, -0.5f},   {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}};

    // bottom
    vertices[20] = {{ 0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}};
    vertices[21] = {{-0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}};
    vertices[22] = {{ 0.5f, -0.5f,  0.5f},  {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}};
    vertices[23] = {{-0.5f, -0.5f,  0.5f},  {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}};

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

            // Front face (counterclockwise order)
            indices.push_back(currentIdx);
            indices.push_back(nextIdx);
            indices.push_back(nextRightIdx);

            // Back face (counterclockwise order)
            indices.push_back(currentIdx);
            indices.push_back(nextRightIdx);
            indices.push_back(currentIdx + 1);
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

static void add_object(duk::scene::Scene* scene,
                       const glm::vec3& position,
                       const glm::quat& rotation,
                       const glm::vec3& scale,
                       duk::renderer::Material* material,
                       duk::renderer::Mesh* mesh) {
    auto obj = scene->add_object();

    auto position3D = obj.add<duk::renderer::Position3D>();
    position3D->value = position;

    auto rotation3D = obj.add<duk::renderer::Rotation3D>();
    rotation3D->value = rotation;

    auto scale3D = obj.add<duk::renderer::Scale3D>();
    scale3D->value = scale;

    auto meshDrawing = obj.add<duk::renderer::MeshDrawing>();
    meshDrawing->mesh = mesh;
    meshDrawing->material = material;
}

static void populate_scene(duk::scene::Scene* scene,
                           uint32_t objCount,
                           duk::renderer::Material* material,
                           duk::renderer::Mesh* mesh
                           ) {

    const auto colCount = static_cast<uint32_t>(sqrt(static_cast<double>(objCount)));
    const auto rowCount = objCount / std::min(colCount, objCount);

    for (int i = 0; i < objCount; i++) {

        const float colPercent = float(i % (colCount)) / float(colCount - 1);
        const float rowPercent = std::floor((float)i / float(colCount)) / float(rowCount - 1);

        add_object(scene,
                   glm::vec3(lerp(-20, 20, colPercent), lerp(-20, 20, rowPercent), glm::linearRand(-50.f, 30.f)),
                   glm::radians(glm::linearRand(glm::vec3(-95.f), glm::vec3(95.f))),
                   glm::linearRand(glm::vec3(0.02f), glm::vec3(1.3f)),
                   material,
                   mesh
                   );

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

    duk::import::Importer::create();

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
        auto imageDataSource = duk::import::Importer::instance()->load_image("images/uv.jpg", duk::rhi::PixelFormat::RGBA8U);

        duk::rhi::RHI::ImageCreateInfo imageCreateInfo = {};
        imageCreateInfo.commandQueue = m_renderer->main_command_queue();
        imageCreateInfo.initialLayout = duk::rhi::Image::Layout::SHADER_READ_ONLY;
        imageCreateInfo.updateFrequency = duk::rhi::Image::UpdateFrequency::STATIC;
        imageCreateInfo.usage = duk::rhi::Image::Usage::SAMPLED;
        imageCreateInfo.imageDataSource = imageDataSource.get();

        auto result = m_renderer->rhi()->create_image(imageCreateInfo);

        if (!result) {
            throw std::runtime_error("failed to create uv image: " + result.error().description());
        }

        m_image = std::move(result.value());
    }

    {
        duk::renderer::ColorShaderDataSource colorShaderDataSource;

        duk::renderer::ColorMaterialCreateInfo colorMaterialCreateInfo = {};
        colorMaterialCreateInfo.renderer = m_renderer.get();
        colorMaterialCreateInfo.shaderDataSource = &colorShaderDataSource;

        m_colorMaterial = std::make_shared<duk::renderer::ColorMaterial>(colorMaterialCreateInfo);
    }

    {
        duk::renderer::PhongShaderDataSource phongShaderDataSource;

        duk::renderer::PhongMaterialCreateInfo phongMaterialCreateInfo = {};
        phongMaterialCreateInfo.renderer = m_renderer.get();
        phongMaterialCreateInfo.shaderDataSource = &phongShaderDataSource;

        const duk::rhi::Sampler sampler = {.filter = duk::rhi::Sampler::Filter::LINEAR, .wrapMode = duk::rhi::Sampler::WrapMode::REPEAT};

        m_phongGreenMaterial = std::make_shared<duk::renderer::PhongMaterial>(phongMaterialCreateInfo);
        m_phongGreenMaterial->update_base_color({0.0f, 1.0f, 0.0f});
        m_phongGreenMaterial->update_shininess(2);
        m_phongGreenMaterial->update_base_color_image(m_image.get(), sampler);

        m_phongBlueMaterial = std::make_shared<duk::renderer::PhongMaterial>(phongMaterialCreateInfo);
        m_phongBlueMaterial->update_base_color({0.0f, 0.0f, 1.0f});
        m_phongBlueMaterial->update_shininess(4);
        m_phongBlueMaterial->update_base_color_image(m_image.get(), sampler);

        m_phongRedMaterial = std::make_shared<duk::renderer::PhongMaterial>(phongMaterialCreateInfo);
        m_phongRedMaterial->update_base_color({1.0f, 0.0f, 0.0f});
        m_phongRedMaterial->update_shininess(8);
        m_phongRedMaterial->update_base_color_image(m_image.get(), sampler);

        m_phongYellowMaterial = std::make_shared<duk::renderer::PhongMaterial>(phongMaterialCreateInfo);
        m_phongYellowMaterial->update_base_color({1.0f, 1.0f, 0.0f});
        m_phongYellowMaterial->update_shininess(16);
        m_phongYellowMaterial->update_base_color_image(m_image.get(), sampler);

        m_phongWhiteMaterial = std::make_shared<duk::renderer::PhongMaterial>(phongMaterialCreateInfo);
        m_phongWhiteMaterial->update_base_color({1.0f, 1.0f, 1.0f});
        m_phongWhiteMaterial->update_shininess(32);
        m_phongWhiteMaterial->update_base_color_image(m_image.get(), sampler);

        m_phongCyanMaterial = std::make_shared<duk::renderer::PhongMaterial>(phongMaterialCreateInfo);
        m_phongCyanMaterial->update_base_color({0.0f, 1.0f, 1.0f});
        m_phongCyanMaterial->update_shininess(64);
        m_phongCyanMaterial->update_base_color_image(m_image.get(), sampler);
    }


    m_scene = std::make_shared<duk::scene::Scene>();

//    detail::populate_scene(m_scene.get(), 250, m_phongPainter.get(), m_phongMaterial.get(), m_cubeMesh.get());
//    detail::populate_scene(m_scene.get(), 250, m_phongPainter.get(), m_phongMaterial.get(), m_quadMesh.get());

//    detail::populate_scene(m_scene.get(), 250, m_phongPainter.get(), m_phongMaterial.get(), m_sphereMesh.get());
//    detail::populate_scene(m_scene.get(), 250, m_phongPainter.get(), m_phongMaterial.get(), m_cubeMesh.get());

    const glm::vec3 scale(5);
    const float offset = 10;

    detail::add_object(
            m_scene.get(),
            glm::vec3(offset, 0, 0),
            glm::quat(),
            scale,
            m_phongWhiteMaterial.get(),
            m_sphereMesh.get()
    );

    detail::add_object(
            m_scene.get(),
            glm::vec3(-offset, 0, 0),
            glm::quat(),
            scale,
            m_phongWhiteMaterial.get(),
            m_cubeMesh.get()
    );

    detail::add_object(
            m_scene.get(),
            glm::vec3(0, offset, 0),
            glm::quat(),
            scale,
            m_phongWhiteMaterial.get(),
            m_sphereMesh.get()
    );

    detail::add_object(
            m_scene.get(),
            glm::vec3(0, -offset, 0),
            glm::quat(),
            scale,
            m_phongWhiteMaterial.get(),
            m_cubeMesh.get()
    );

    detail::add_object(
            m_scene.get(),
            glm::vec3(0, 0, offset),
            glm::quat(),
            scale,
            m_phongWhiteMaterial.get(),
            m_sphereMesh.get()
    );

    detail::add_object(
            m_scene.get(),
            glm::vec3(0, 0, -offset),
            glm::quat(),
            scale,
            m_phongWhiteMaterial.get(),
            m_cubeMesh.get()
    );

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

    // lights
    {
        auto object = m_scene->add_object();
        auto pointLight = object.add<duk::renderer::PointLight>();
        pointLight->value.color = glm::vec3(0.5f, 1.f, 0.2f);
        pointLight->value.intensity = 5.0f;

        auto position3D = object.add<duk::renderer::Position3D>();
        position3D->value = glm::vec3(offset * 2, 5, offset);
    }

    {
        auto object = m_scene->add_object();
        auto pointLight = object.add<duk::renderer::PointLight>();
        pointLight->value.color = glm::vec3(0.9f, 0.3f, 0.459f);
        pointLight->value.intensity = 8.0f;

        auto position3D = object.add<duk::renderer::Position3D>();
        position3D->value = glm::vec3(-offset * 2, -5, 10);
    }

    {
        auto object = m_scene->add_object();
        auto directionalLight = object.add<duk::renderer::DirectionalLight>();
        directionalLight->value.color = glm::vec3(1, 1, 1);
        directionalLight->value.intensity = 0.85f;

        auto rotation3D = object.add<duk::renderer::Rotation3D>();
        rotation3D->value = glm::quatLookAt(glm::vec3(0, -1, 0), glm::vec3(0, 1, 0));
    }
}

Application::~Application() {
    duk::import::Importer::destroy();
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

