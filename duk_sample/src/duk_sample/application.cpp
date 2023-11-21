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

    auto meshPool = m_engine->mesh_pool();
    const glm::vec3 scale(5);
    const float offset = 10;

    detail::add_object(
            scene,
            glm::vec3(offset, 0, 0),
            glm::quat(),
            scale,
            m_phongWhiteMaterial.get(),
            meshPool->sphere().get()
    );

    detail::add_object(
            scene,
            glm::vec3(-offset, 0, 0),
            glm::quat(),
            scale,
            m_phongWhiteMaterial.get(),
            meshPool->cube().get()
    );

    detail::add_object(
            scene,
            glm::vec3(0, offset, 0),
            glm::quat(),
            scale,
            m_phongWhiteMaterial.get(),
            meshPool->sphere().get()
    );

    detail::add_object(
            scene,
            glm::vec3(0, -offset, 0),
            glm::quat(),
            scale,
            m_phongWhiteMaterial.get(),
            meshPool->cube().get()
    );

    detail::add_object(
            scene,
            glm::vec3(0, 0, offset),
            glm::quat(),
            scale,
            m_phongWhiteMaterial.get(),
            meshPool->sphere().get()
    );

    detail::add_object(
            scene,
            glm::vec3(0, 0, -offset),
            glm::quat(),
            scale,
            m_phongWhiteMaterial.get(),
            meshPool->cube().get()
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

