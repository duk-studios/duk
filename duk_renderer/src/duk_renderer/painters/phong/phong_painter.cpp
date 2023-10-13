/// 12/10/2023
/// phong_painter.cpp

#include <duk_renderer/painters/phong/phong_painter.h>
#include <duk_renderer/painters/phong/phong_palette.h>
#include <duk_tools/file.h>

namespace duk::renderer {

namespace detail {

static duk::rhi::StdShaderDataSource phong_shader_data_source() {
    duk::rhi::StdShaderDataSource shaderDataSource;
    shaderDataSource.insert_spir_v_code(duk::rhi::Shader::Module::VERTEX, duk::tools::File::load_bytes("spv/phong/phong.vert.spv"));
    shaderDataSource.insert_spir_v_code(duk::rhi::Shader::Module::FRAGMENT, duk::tools::File::load_bytes("spv/phong/phong.frag.spv"));

    shaderDataSource.insert_vertex_attribute(duk::rhi::VertexAttribute::Format::VEC3);
    shaderDataSource.insert_vertex_attribute(duk::rhi::VertexAttribute::Format::VEC3);
    shaderDataSource.insert_vertex_attribute(duk::rhi::VertexAttribute::Format::VEC2);

    {
        duk::rhi::DescriptorSetDescription descriptorSetDescription;
        auto& cameraBinding = descriptorSetDescription.bindings.emplace_back();
        cameraBinding.type = rhi::DescriptorType::UNIFORM_BUFFER;
        cameraBinding.moduleMask = duk::rhi::Shader::Module::VERTEX;

        auto& transformBinding = descriptorSetDescription.bindings.emplace_back();
        transformBinding.type = duk::rhi::DescriptorType::STORAGE_BUFFER;
        transformBinding.moduleMask = duk::rhi::Shader::Module::VERTEX;

        auto& materialBinding = descriptorSetDescription.bindings.emplace_back();
        materialBinding.type = duk::rhi::DescriptorType::UNIFORM_BUFFER;
        materialBinding.moduleMask = duk::rhi::Shader::Module::FRAGMENT;

        shaderDataSource.insert_descriptor_set_description(descriptorSetDescription);
    }

    shaderDataSource.update_hash();

    return shaderDataSource;
}
}

PhongPainter::PhongPainter(const PhongPainterCreateInfo& phongPainterCreateInfo) :
    Painter(phongPainterCreateInfo.rhi),
    m_commandQueue(phongPainterCreateInfo.commandQueue) {
    m_shaderDataSource = detail::phong_shader_data_source();
    init_shader(&m_shaderDataSource);
}

PhongPainter::~PhongPainter() = default;

const duk::rhi::DescriptorSetDescription& PhongPainter::descriptor_set_description() const {
    return m_shaderDataSource.descriptor_set_descriptions().at(0);
}

std::shared_ptr<PhongPalette> PhongPainter::make_palette() const {
    PhongPaletteCreateInfo colorPaletteCreateInfo = {};
    colorPaletteCreateInfo.rhi = m_rhi;
    colorPaletteCreateInfo.commandQueue = m_commandQueue;
    colorPaletteCreateInfo.painter = this;
    return std::make_shared<PhongPalette>(colorPaletteCreateInfo);
}

}