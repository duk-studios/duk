/// 11/10/2023
/// custom_brush.cpp

#include <duk_renderer/brushes/generic_brush.h>

namespace duk::renderer {

GenericBrush::GenericBrush(GenericBrush::DrawFunction drawFunction) : m_drawFunction(std::move(drawFunction)) {
}

GenericBrush::~GenericBrush() = default;

void GenericBrush::draw(duk::rhi::CommandBuffer* commandBuffer, size_t instanceCount, size_t firstInstance) {
    if (m_drawFunction) {
        m_drawFunction(commandBuffer, instanceCount, firstInstance);
    }
}

}// namespace duk::renderer