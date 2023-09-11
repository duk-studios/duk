/// 20/08/2023
/// color_shader.cpp

#include <duk_renderer/painters/color/color_painter.h>
#include <duk_renderer/mesh.h>

#include <fstream>

namespace duk::renderer {

namespace detail {

static std::vector<uint8_t> load_bytes(const char* filepath) {
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

static duk::rhi::StdShaderDataSource color_shader_data_source() {
    duk::rhi::StdShaderDataSource shaderDataSource;
    shaderDataSource.insert_spir_v_code(duk::rhi::Shader::Module::VERTEX, load_bytes("spv/color/color.vert.spv"));
    shaderDataSource.insert_spir_v_code(duk::rhi::Shader::Module::FRAGMENT, load_bytes("spv/color/color.frag.spv"));

    shaderDataSource.insert_vertex_attribute(duk::rhi::VertexAttribute::Format::VEC2);
    shaderDataSource.insert_vertex_attribute(duk::rhi::VertexAttribute::Format::VEC4);
    shaderDataSource.insert_vertex_attribute(duk::rhi::VertexAttribute::Format::VEC2);

    duk::rhi::DescriptorSetDescription descriptorSetDescription;
    auto& transformBinding = descriptorSetDescription.bindings.emplace_back();
    transformBinding.type = rhi::DescriptorType::UNIFORM_BUFFER;
    transformBinding.moduleMask = duk::rhi::Shader::Module::VERTEX;

    auto& uniformBinding = descriptorSetDescription.bindings.emplace_back();
    uniformBinding.type = duk::rhi::DescriptorType::UNIFORM_BUFFER;
    uniformBinding.moduleMask = duk::rhi::Shader::Module::FRAGMENT;

    shaderDataSource.insert_descriptor_set_description(descriptorSetDescription);

    shaderDataSource.update_hash();

    return shaderDataSource;
}

}

ColorPainter::ColorPainter(const ColorPainterCreateInfo& colorPainterCreateInfo) :
    Painter(colorPainterCreateInfo.rhi) {
    m_shaderDataSource = detail::color_shader_data_source();
    init_shader(&m_shaderDataSource);
}

void ColorPainter::paint(duk::rhi::CommandBuffer* commandBuffer, const ColorPainter::PaintParams& params) {

    commandBuffer->bind_graphics_pipeline(pipeline_for_params(params));

    commandBuffer->bind_descriptor_set(params.instanceDescriptorSet, 0);

    params.mesh->draw(commandBuffer);
}

const duk::rhi::ShaderDataSource* ColorPainter::shader_data_source() const {
    return &m_shaderDataSource;
}

const duk::rhi::DescriptorSetDescription& ColorPainter::instance_descriptor_set_description() const {
    return m_shaderDataSource.descriptor_set_descriptions().at(0);
}

}