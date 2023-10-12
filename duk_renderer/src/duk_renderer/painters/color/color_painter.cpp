/// 20/08/2023
/// color_shader.cpp

#include <duk_renderer/painters/color/color_painter.h>
#include <duk_renderer/painters/color/color_palette.h>

#include <duk_tools/file.h>

namespace duk::renderer {

namespace detail {

static duk::rhi::StdShaderDataSource fullscreen_shader_data_source() {
    duk::rhi::StdShaderDataSource shaderDataSource;
    shaderDataSource.insert_spir_v_code(duk::rhi::Shader::Module::VERTEX, duk::tools::File::load_bytes("spv/color/color.vert.spv"));
    shaderDataSource.insert_spir_v_code(duk::rhi::Shader::Module::FRAGMENT, duk::tools::File::load_bytes("spv/color/color.frag.spv"));

    shaderDataSource.insert_vertex_attribute(duk::rhi::VertexAttribute::Format::VEC3);
    shaderDataSource.insert_vertex_attribute(duk::rhi::VertexAttribute::Format::VEC4);
    shaderDataSource.insert_vertex_attribute(duk::rhi::VertexAttribute::Format::VEC2);

    // global
    {
        duk::rhi::DescriptorSetDescription globalSetDescription;
        auto& cameraBinding = globalSetDescription.bindings.emplace_back();
        cameraBinding.type = rhi::DescriptorType::UNIFORM_BUFFER;
        cameraBinding.moduleMask = duk::rhi::Shader::Module::VERTEX;

        auto& materialBinding = globalSetDescription.bindings.emplace_back();
        materialBinding.type = duk::rhi::DescriptorType::UNIFORM_BUFFER;
        materialBinding.moduleMask = duk::rhi::Shader::Module::FRAGMENT;

        shaderDataSource.insert_descriptor_set_description(globalSetDescription);
    }

    // instance
    {
        duk::rhi::DescriptorSetDescription instanceSetDescription;
        auto& transformBinding = instanceSetDescription.bindings.emplace_back();
        transformBinding.type = duk::rhi::DescriptorType::STORAGE_BUFFER;
        transformBinding.moduleMask = duk::rhi::Shader::Module::VERTEX;

        shaderDataSource.insert_descriptor_set_description(instanceSetDescription);
    }

    shaderDataSource.update_hash();

    return shaderDataSource;
}

}

ColorPainter::ColorPainter(const ColorPainterCreateInfo& colorPainterCreateInfo) :
    Painter(colorPainterCreateInfo.rhi),
    m_commandQueue(colorPainterCreateInfo.commandQueue) {
    m_shaderDataSource = detail::fullscreen_shader_data_source();
    init_shader(&m_shaderDataSource);
}

const duk::rhi::DescriptorSetDescription& ColorPainter::global_descriptor_set_description() const {
    return m_shaderDataSource.descriptor_set_descriptions().at(0);
}

const duk::rhi::DescriptorSetDescription& ColorPainter::instance_descriptor_set_description() const {
    return m_shaderDataSource.descriptor_set_descriptions().at(1);
}

std::shared_ptr<ColorPalette> ColorPainter::make_palette() const {
    duk::renderer::ColorPaletteCreateInfo colorPaletteCreateInfo = {};
    colorPaletteCreateInfo.rhi = m_rhi;
    colorPaletteCreateInfo.commandQueue = m_commandQueue;
    colorPaletteCreateInfo.painter = this;
    return std::make_shared<duk::renderer::ColorPalette>(colorPaletteCreateInfo);
}

}