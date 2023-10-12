/// 05/10/2023
/// fullscreen_painter.cpp

#include <duk_renderer/painters/fullscreen/fullscreen_painter.h>
#include <duk_renderer/painters/fullscreen/fullscreen_palette.h>

#include <duk_tools/file.h>

namespace duk::renderer {

namespace detail {

static duk::rhi::StdShaderDataSource fullscreen_shader_data_source() {
    duk::rhi::StdShaderDataSource shaderDataSource;
    shaderDataSource.insert_spir_v_code(duk::rhi::Shader::Module::VERTEX, duk::tools::File::load_bytes("spv/fullscreen/fullscreen.vert.spv"));
    shaderDataSource.insert_spir_v_code(duk::rhi::Shader::Module::FRAGMENT, duk::tools::File::load_bytes("spv/fullscreen/fullscreen.frag.spv"));

    {
        duk::rhi::DescriptorSetDescription globalSetDescription;

        auto& textureBinding = globalSetDescription.bindings.emplace_back();
        textureBinding.type = duk::rhi::DescriptorType::IMAGE_SAMPLER;
        textureBinding.moduleMask = duk::rhi::Shader::Module::FRAGMENT;

        shaderDataSource.insert_descriptor_set_description(globalSetDescription);
    }

    shaderDataSource.update_hash();

    return shaderDataSource;
}

}

FullscreenPainter::FullscreenPainter(const FullscreenPainterCreateInfo& fullscreenPainterCreateInfo) :
    Painter(fullscreenPainterCreateInfo.rhi),
    m_commandQueue(fullscreenPainterCreateInfo.commandQueue) {
    m_shaderDataSource = detail::fullscreen_shader_data_source();
    init_shader(&m_shaderDataSource);
}

FullscreenPainter::~FullscreenPainter() = default;

const duk::rhi::DescriptorSetDescription& FullscreenPainter::descriptor_set_description() const {
    return m_shaderDataSource.descriptor_set_descriptions().at(0);
}

std::shared_ptr<FullscreenPalette> FullscreenPainter::make_palette() const {
    FullscreenPaletteCreateInfo fullscreenPaletteCreateInfo = {};
    fullscreenPaletteCreateInfo.rhi = m_rhi;
    fullscreenPaletteCreateInfo.commandQueue = m_commandQueue;
    fullscreenPaletteCreateInfo.painter = this;
    return std::make_shared<FullscreenPalette>(fullscreenPaletteCreateInfo);
}

}