/// 12/09/2023
/// color_material.cpp

#include <duk_renderer/renderer.h>
#include <duk_renderer/resources/materials/color/color_material.h>
#include <duk_renderer/resources/materials/globals/global_descriptors.h>
#include <duk_renderer/components/transform.h>

namespace duk::renderer {

namespace detail {

static const ColorShaderDataSource kColorShaderDataSource;

}

ColorMaterialDataSource::ColorMaterialDataSource() :
    color(glm::vec4(1)),
    colorTexture() {

}

const duk::rhi::ShaderDataSource* ColorMaterialDataSource::shader_data_source() const {
    return &detail::kColorShaderDataSource;
}

std::unique_ptr<MaterialDescriptorSet> ColorMaterialDataSource::create_descriptor_set(const MaterialDescriptorSetCreateInfo& materialDescriptorSetCreateInfo) const {
    ColorMaterialDescriptorSetCreateInfo colorMaterialDescriptorSetCreateInfo = {};
    colorMaterialDescriptorSetCreateInfo.renderer = materialDescriptorSetCreateInfo.renderer;
    colorMaterialDescriptorSetCreateInfo.colorMaterialDataSource = this;
    return std::make_unique<ColorMaterialDescriptorSet>(colorMaterialDescriptorSetCreateInfo);
}

duk::hash::Hash ColorMaterialDataSource::calculate_hash() const {
    duk::hash::Hash hash = 0;
    duk::hash::hash_combine(hash, color);
    duk::hash::hash_combine(hash, colorTexture);
    return hash;

}

ColorInstanceBuffer::ColorInstanceBuffer(const ColorInstanceBufferCreateInfo& colorInstanceBufferCreateInfo) {
    auto rhi = colorInstanceBufferCreateInfo.renderer->rhi();
    auto commandQueue = colorInstanceBufferCreateInfo.renderer->main_command_queue();

    StorageBufferCreateInfo transformSBOCreateInfo = {};
    transformSBOCreateInfo.rhi = rhi;
    transformSBOCreateInfo.commandQueue = commandQueue;
    transformSBOCreateInfo.initialSize = 1;
    m_transformSBO = std::make_unique<StorageBuffer<ColorDescriptors::Instance>>(transformSBOCreateInfo);
}

void ColorInstanceBuffer::insert(const scene::Object& object) {
    auto& transform = m_transformSBO->next();
    transform.model = model_matrix_3d(object);
}

void ColorInstanceBuffer::clear() {
    m_transformSBO->clear();
}

void ColorInstanceBuffer::flush() {
    m_transformSBO->flush();
}

StorageBuffer<ColorDescriptors::Instance>* ColorInstanceBuffer::transform_buffer() {
    return m_transformSBO.get();
}

ColorMaterialDescriptorSet::ColorMaterialDescriptorSet(const ColorMaterialDescriptorSetCreateInfo& colorMaterialDescriptorSetCreateInfo) {
    auto renderer = colorMaterialDescriptorSetCreateInfo.renderer;
    auto materialDataSource = colorMaterialDescriptorSetCreateInfo.colorMaterialDataSource;
    auto shaderDataSource = materialDataSource->shader_data_source();
    auto rhi = renderer->rhi();
    auto commandQueue = renderer->main_command_queue();

    {
        duk::rhi::RHI::DescriptorSetCreateInfo descriptorSetCreateInfo = {};
        descriptorSetCreateInfo.description = shaderDataSource->descriptor_set_descriptions().at(0);
        m_descriptorSet = rhi->create_descriptor_set(descriptorSetCreateInfo);
    }
    {
        ColorInstanceBufferCreateInfo colorInstanceBufferCreateInfo = {};
        colorInstanceBufferCreateInfo.renderer = renderer;
        m_instanceBuffer = std::make_unique<ColorInstanceBuffer>(colorInstanceBufferCreateInfo);
        m_descriptorSet->set(ColorDescriptors::uInstances, m_instanceBuffer->transform_buffer()->descriptor());
    }
    {
        UniformBufferCreateInfo<ColorDescriptors::Material> materialUBOCreateInfo = {};
        materialUBOCreateInfo.rhi = rhi;
        materialUBOCreateInfo.commandQueue = commandQueue;
        materialUBOCreateInfo.initialData.color = materialDataSource->color;
        m_materialUBO = std::make_unique<UniformBuffer<ColorDescriptors::Material>>(materialUBOCreateInfo);
        m_descriptorSet->set(ColorDescriptors::uMaterial, m_materialUBO->descriptor());
    }
    set_color(materialDataSource->color);
    set_color_texture(materialDataSource->colorTexture);
}

void ColorMaterialDescriptorSet::set_color(const glm::vec4& color) {
    m_materialUBO->data().color = color;
    m_materialUBO->flush();
}

void ColorMaterialDescriptorSet::set_color_texture(const Texture& colorTexture) {
    m_colorTexture = colorTexture;
}

void ColorMaterialDescriptorSet::bind(duk::rhi::CommandBuffer* commandBuffer, const DrawParams& params) {
    // updates current camera UBO
    m_descriptorSet->set(ColorDescriptors::uCamera, params.globalDescriptors->camera_ubo()->descriptor());

    // these might have changed
    m_descriptorSet->set(ColorDescriptors::uBaseColor, m_colorTexture.descriptor());

    // updates descriptor set in case some descriptor changed
    m_descriptorSet->flush();

    commandBuffer->bind_descriptor_set(m_descriptorSet.get(), 0);
}

uint32_t ColorMaterialDescriptorSet::size() const {
    return ColorDescriptors::kDescriptorCount;
}

ImageResource& ColorMaterialDescriptorSet::image_at(uint32_t index) {
    switch (index) {
        case ColorDescriptors::uBaseColor:
            return m_colorTexture.image();
        default:
            throw std::invalid_argument(fmt::format("Descriptor at index {} is not an image", index));
    }
}

bool ColorMaterialDescriptorSet::is_image(uint32_t index) {
    return index == ColorDescriptors::uBaseColor;
}

InstanceBuffer* ColorMaterialDescriptorSet::instance_buffer() {
    return m_instanceBuffer.get();
}

}
