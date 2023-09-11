/// 08/09/2023
/// canvas.cpp

#include <duk_renderer/canvas.h>

namespace duk::renderer {

Canvas::Canvas(const CanvasCreateInfo& canvasCreateInfo) :
    m_width(canvasCreateInfo.width),
    m_height(canvasCreateInfo.height),
    m_renderPass(canvasCreateInfo.renderPass) {

}

uint32_t Canvas::width() const {
    return m_width;
}

uint32_t Canvas::height() const {
    return m_height;
}

duk::rhi::RenderPass* Canvas::render_pass() const {
    return m_renderPass.get();
}

}