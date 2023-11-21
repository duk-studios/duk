/// 03/06/2023
/// application.cpp

#include <duk_sample/application.h>
#include <duk_sample/camera_system.h>
#include <duk_engine/systems/render_system.h>
#include <duk_renderer/vertex_types.h>
#include <duk_renderer/materials/painter.h>
#include <duk_renderer/components/mesh_drawing.h>
#include <duk_renderer/components/transform.h>
#include <duk_renderer/components/lighting.h>
#include <duk_import/importer.h>

#include <glm/gtc/random.hpp>

#include <cmath>

namespace duk::sample {

namespace detail {

template<typename T>
T lerp(T from, T to, float percent) {
    return (from * (1 - percent)) + (to * percent);
}

using DefaultMeshDataSource = duk::renderer::MeshDataSourceT<duk::renderer::VertexNormalUV, uint16_t>;

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

    const auto rows = segments;
    const auto columns = segments;

    std::vector<DefaultMeshDataSource::VertexType> vertices;

    for (int row = 0; row <= rows; ++row) {
        float rowAngle = static_cast<float>(row) * glm::pi<float>() / static_cast<float>(rows);
        float rowSin = std::sin(rowAngle);
        float rowCos = std::cos(rowAngle);

        for (int column = 0; column <= columns; ++column) {
            float columnAngle = static_cast<float>(column) * 2.0f * glm::pi<float>() / static_cast<float>(columns);
            float columnSin = std::sin(columnAngle);
            float columnCos = std::cos(columnAngle);

            float x = rowSin * columnCos;
            float y = rowCos;
            float z = rowSin * columnSin;
            float u = 1 - (static_cast<float>(column) / static_cast<float>(columns));
            float v = static_cast<float>(row) / static_cast<float>(rows);

            glm::vec3 position(x, y, z);
            glm::vec3 normal(x, y, z);
            glm::vec2 uv(u, v);

            vertices.push_back({position, normal, uv});
        }
    }

    meshDataSource.insert_vertices(vertices.begin(), vertices.end());

    std::vector<DefaultMeshDataSource::IndexType> indices;

    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            int firstVertexIndex = column + (row * (columns + 1));
            int secondVertexIndex = firstVertexIndex + 1;
            int thirdVertexIndex = column + ((row + 1) * (columns + 1));
            int fourthVertexIndex = thirdVertexIndex + 1;

            indices.push_back(firstVertexIndex);
            indices.push_back(thirdVertexIndex);
            indices.push_back(secondVertexIndex);

            indices.push_back(thirdVertexIndex);
            indices.push_back(fourthVertexIndex);
            indices.push_back(secondVertexIndex);
        }
    }

    meshDataSource.insert_indices(indices.begin(), indices.end());

    meshDataSource.update_hash();

    return meshDataSource;
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

}

Application::Application(const ApplicationCreateInfo& applicationCreateInfo) {

    duk::engine::EngineCreateInfo engineCreateInfo = {};
    engineCreateInfo.applicationName = applicationCreateInfo.name;

    m_engine = std::make_unique<duk::engine::Engine>(engineCreateInfo);

    auto renderer = m_engine->renderer();

    duk::renderer::MeshBufferPoolCreateInfo meshBufferPoolCreateInfo = {};
    meshBufferPoolCreateInfo.rhi = renderer->rhi();
    meshBufferPoolCreateInfo.commandQueue = renderer->main_command_queue();

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
        auto sphereDataSource = detail::sphere_mesh_data_source(32);

        m_sphereMesh = m_meshBufferPool->create_mesh(&sphereDataSource);
    }


    {
        duk::renderer::ColorShaderDataSource colorShaderDataSource;

        duk::renderer::ColorMaterialCreateInfo colorMaterialCreateInfo = {};
        colorMaterialCreateInfo.renderer = renderer;
        colorMaterialCreateInfo.shaderDataSource = &colorShaderDataSource;

        m_colorMaterial = std::make_shared<duk::renderer::ColorMaterial>(colorMaterialCreateInfo);
    }

    {
        auto imagePool = m_engine->image_pool();
        auto goldColorImage = imagePool->load("images/gold_basecolor.png", duk::rhi::PixelFormat::RGBA8U);
        auto goldSpecularImage = imagePool->load("images/gold_specular.png", duk::rhi::PixelFormat::R8U);

        duk::renderer::PhongShaderDataSource phongShaderDataSource;

        duk::renderer::PhongMaterialCreateInfo phongMaterialCreateInfo = {};
        phongMaterialCreateInfo.renderer = renderer;
        phongMaterialCreateInfo.shaderDataSource = &phongShaderDataSource;

        const duk::rhi::Sampler sampler = {.filter = duk::rhi::Sampler::Filter::NEAREST, .wrapMode = duk::rhi::Sampler::WrapMode::CLAMP_TO_BORDER};

        m_phongGreenMaterial = std::make_shared<duk::renderer::PhongMaterial>(phongMaterialCreateInfo);
        m_phongGreenMaterial->update_base_color({0.0f, 1.0f, 0.0f});
        m_phongGreenMaterial->update_shininess(2);
        m_phongGreenMaterial->update_base_color_image(goldColorImage.get(), sampler);
        m_phongGreenMaterial->update_shininess_image(goldSpecularImage.get(), sampler);

        m_phongBlueMaterial = std::make_shared<duk::renderer::PhongMaterial>(phongMaterialCreateInfo);
        m_phongBlueMaterial->update_base_color({0.0f, 0.0f, 1.0f});
        m_phongBlueMaterial->update_shininess(4);
        m_phongBlueMaterial->update_base_color_image(goldColorImage.get(), sampler);
        m_phongBlueMaterial->update_shininess_image(goldSpecularImage.get(), sampler);

        m_phongRedMaterial = std::make_shared<duk::renderer::PhongMaterial>(phongMaterialCreateInfo);
        m_phongRedMaterial->update_base_color({1.0f, 0.0f, 0.0f});
        m_phongRedMaterial->update_shininess(8);
        m_phongRedMaterial->update_base_color_image(goldColorImage.get(), sampler);
        m_phongRedMaterial->update_shininess_image(goldSpecularImage.get(), sampler);

        m_phongYellowMaterial = std::make_shared<duk::renderer::PhongMaterial>(phongMaterialCreateInfo);
        m_phongYellowMaterial->update_base_color({1.0f, 1.0f, 0.0f});
        m_phongYellowMaterial->update_shininess(16);
        m_phongYellowMaterial->update_base_color_image(goldColorImage.get(), sampler);
        m_phongYellowMaterial->update_shininess_image(goldSpecularImage.get(), sampler);

        m_phongWhiteMaterial = std::make_shared<duk::renderer::PhongMaterial>(phongMaterialCreateInfo);
        m_phongWhiteMaterial->update_base_color({0.3f, 0.3f, 0.3f});
        m_phongWhiteMaterial->update_shininess(4);
        m_phongWhiteMaterial->update_base_color_image(imagePool->white_image().get(), sampler);
        m_phongWhiteMaterial->update_shininess_image(goldSpecularImage.get(), sampler);

        m_phongCyanMaterial = std::make_shared<duk::renderer::PhongMaterial>(phongMaterialCreateInfo);
        m_phongCyanMaterial->update_base_color({0.0f, 1.0f, 1.0f});
        m_phongCyanMaterial->update_shininess(64);
        m_phongCyanMaterial->update_base_color_image(goldColorImage.get(), sampler);
        m_phongCyanMaterial->update_shininess_image(goldSpecularImage.get(), sampler);
    }


    auto scene = m_engine->scene();

//    detail::populate_scene(m_scene.get(), 250, m_phongPainter.get(), m_phongMaterial.get(), m_cubeMesh.get());
//    detail::populate_scene(m_scene.get(), 250, m_phongPainter.get(), m_phongMaterial.get(), m_quadMesh.get());

//    detail::populate_scene(m_scene.get(), 250, m_phongPainter.get(), m_phongMaterial.get(), m_sphereMesh.get());
//    detail::populate_scene(m_scene.get(), 250, m_phongPainter.get(), m_phongMaterial.get(), m_cubeMesh.get());

    const glm::vec3 scale(5);
    const float offset = 10;

    detail::add_object(
            scene,
            glm::vec3(offset, 0, 0),
            glm::quat(),
            scale,
            m_phongWhiteMaterial.get(),
            m_sphereMesh.get()
    );

    detail::add_object(
            scene,
            glm::vec3(-offset, 0, 0),
            glm::quat(),
            scale,
            m_phongWhiteMaterial.get(),
            m_cubeMesh.get()
    );

    detail::add_object(
            scene,
            glm::vec3(0, offset, 0),
            glm::quat(),
            scale,
            m_phongWhiteMaterial.get(),
            m_sphereMesh.get()
    );

    detail::add_object(
            scene,
            glm::vec3(0, -offset, 0),
            glm::quat(),
            scale,
            m_phongWhiteMaterial.get(),
            m_cubeMesh.get()
    );

    detail::add_object(
            scene,
            glm::vec3(0, 0, offset),
            glm::quat(),
            scale,
            m_phongWhiteMaterial.get(),
            m_sphereMesh.get()
    );

    detail::add_object(
            scene,
            glm::vec3(0, 0, -offset),
            glm::quat(),
            scale,
            m_phongWhiteMaterial.get(),
            m_cubeMesh.get()
    );


    // lights
    {
        auto object = scene->add_object();
        auto pointLight = object.add<duk::renderer::PointLight>();
        pointLight->value.color = glm::vec3(0.5f, 1.f, 0.2f);
        pointLight->value.intensity = 5.0f;

        auto position3D = object.add<duk::renderer::Position3D>();
        position3D->value = glm::vec3(offset * 2, 5, offset);
    }

    {
        auto object = scene->add_object();
        auto pointLight = object.add<duk::renderer::PointLight>();
        pointLight->value.color = glm::vec3(0.9f, 0.3f, 0.459f);
        pointLight->value.intensity = 8.0f;

        auto position3D = object.add<duk::renderer::Position3D>();
        position3D->value = glm::vec3(-offset * 2, -5, 10);
    }

    {
        auto object = scene->add_object();
        auto directionalLight = object.add<duk::renderer::DirectionalLight>();
        directionalLight->value.color = glm::vec3(1, 1, 1);
        directionalLight->value.intensity = 0.85f;

        auto rotation3D = object.add<duk::renderer::Rotation3D>();
        rotation3D->value = glm::quatLookAt(glm::vec3(0, -1, 0), glm::vec3(0, 1, 0));
    }

    auto systems = m_engine->systems();
    systems->add_system<CameraSystem>(*m_engine);
    systems->add_system<duk::engine::RenderSystem>(*m_engine);
}

Application::~Application() {

}

void Application::run() {
    m_engine->run();
}

}

