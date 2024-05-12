//
// Created by rov on 11/21/2023.
//

#include <duk_renderer/image/image_pool.h>
#include <duk_renderer/material/material_pool.h>
#include <duk_renderer/shader/shader_pipeline_pool.h>

namespace duk::renderer {

namespace detail {

static void add_transform_binding(MaterialData& materialData) {
    auto& transforms = materialData.bindings.emplace_back();
    transforms.name = "uTransform";
    transforms.type = BindingType::INSTANCE;
}

}

MaterialPool::MaterialPool(const MaterialPoolCreateInfo& materialPoolCreateInfo)
    : m_renderer(materialPoolCreateInfo.renderer) {
}

MaterialResource MaterialPool::create(duk::resource::Id resourceId, const MaterialData* materialData) {
    MaterialCreateInfo materialCreateInfo = {};
    materialCreateInfo.renderer = m_renderer;
    materialCreateInfo.materialData = materialData;
    return insert(resourceId, std::make_shared<Material>(materialCreateInfo));
}

MaterialResource MaterialPool::create_color(const duk::resource::Pools& pools, duk::resource::Id resourceId) {
    MaterialData materialData = {};
    materialData.shader = pools.get<ShaderPipelinePool>()->color();
    detail::add_transform_binding(materialData);
    {
        auto& properties = materialData.bindings.emplace_back();
        properties.name = "uProperties";
        properties.type = BindingType::INSTANCE;
        properties.data = []() {
            auto bindingData = std::make_unique<BufferBinding>();
            {
                auto& color = bindingData->members.emplace_back();
                color.name = "color";
                color.type = BufferBinding::Member::Type::VEC4;
                color.data.vec4Value = glm::vec4(1.0f);
            }
            return bindingData;
        }();
    }
    {
        auto& baseColor = materialData.bindings.emplace_back();
        baseColor.type = BindingType::IMAGE_SAMPLER;
        baseColor.name = "uBaseColor";
        baseColor.data = [&pools]() -> std::unique_ptr<BindingData> {
            auto imageSamplerBinding = std::make_unique<ImageSamplerBinding>();
            imageSamplerBinding->image = pools.get<ImagePool>()->white_image();
            imageSamplerBinding->sampler = kDefaultTextureSampler;
            return imageSamplerBinding;
        }();
    }
    return create(resourceId, &materialData);
}

MaterialResource MaterialPool::create_phong(const duk::resource::Pools& pools, duk::resource::Id resourceId) {
    MaterialData materialData = {};
    materialData.shader = pools.get<ShaderPipelinePool>()->phong();
    detail::add_transform_binding(materialData);
    {
        auto& properties = materialData.bindings.emplace_back();
        properties.name = "uProperties";
        properties.type = BindingType::INSTANCE;
        properties.data = []() {
            auto bindingData = std::make_unique<BufferBinding>();
            {
                auto& color = bindingData->members.emplace_back();
                color.name = "color";
                color.type = BufferBinding::Member::Type::VEC4;
                color.data.vec4Value = glm::vec4(1.0f);
            }
            {
                auto& shininess = bindingData->members.emplace_back();
                shininess.name = "shininess";
                shininess.type = BufferBinding::Member::Type::FLOAT;
                shininess.data.floatValue = 32.0f;
            }
            return bindingData;
        }();
    }
    {
        auto& baseColor = materialData.bindings.emplace_back();
        baseColor.type = BindingType::IMAGE_SAMPLER;
        baseColor.name = "uBaseColor";
        baseColor.data = [&pools]() -> std::unique_ptr<BindingData> {
            auto imageSamplerBinding = std::make_unique<ImageSamplerBinding>();
            imageSamplerBinding->image = pools.get<ImagePool>()->white_image();
            imageSamplerBinding->sampler = kDefaultTextureSampler;
            return imageSamplerBinding;
        }();
    }
    {
        auto& specular = materialData.bindings.emplace_back();
        specular.type = BindingType::IMAGE_SAMPLER;
        specular.name = "uSpecular";
        specular.data = [&pools]() -> std::unique_ptr<BindingData> {
            auto imageSamplerBinding = std::make_unique<ImageSamplerBinding>();
            imageSamplerBinding->image = pools.get<ImagePool>()->white_image();
            imageSamplerBinding->sampler = kDefaultTextureSampler;
            return imageSamplerBinding;
        }();
    }
    return create(resourceId, &materialData);
}

}// namespace duk::renderer
