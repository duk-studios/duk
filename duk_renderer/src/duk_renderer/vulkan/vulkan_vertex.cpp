/// 21/05/2023
/// vulkan_vertex.cpp

#include <duk_renderer/vulkan/vulkan_vertex.h>

namespace duk::renderer {

VkFormat convert_vertex_attribute_format(VertexAttribute::Format format) {
    VkFormat converted;
    switch (format) {
        case VertexAttribute::Format::UNDEFINED: converted = VK_FORMAT_UNDEFINED; break;
        case VertexAttribute::Format::INT8: converted = VK_FORMAT_R8_SINT; break;
        case VertexAttribute::Format::UINT8: converted = VK_FORMAT_R8_UINT; break;
        case VertexAttribute::Format::INT16: converted = VK_FORMAT_R16_SINT; break;
        case VertexAttribute::Format::UINT16: converted = VK_FORMAT_R16_UINT; break;
        case VertexAttribute::Format::INT32: converted = VK_FORMAT_R32_SINT; break;
        case VertexAttribute::Format::UINT32: converted = VK_FORMAT_R32_UINT; break;
        case VertexAttribute::Format::FLOAT32: converted = VK_FORMAT_R32_SFLOAT; break;
        case VertexAttribute::Format::VEC2: converted = VK_FORMAT_R32G32_SFLOAT; break;
        case VertexAttribute::Format::VEC3: converted = VK_FORMAT_R32G32B32_SFLOAT; break;
        case VertexAttribute::Format::VEC4: converted = VK_FORMAT_R32G32B32A32_SFLOAT; break;
    }
    return converted;
}

}