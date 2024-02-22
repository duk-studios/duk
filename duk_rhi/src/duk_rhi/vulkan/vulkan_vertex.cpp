/// 21/05/2023
/// vulkan_vertex.cpp

#include <duk_rhi/vulkan/vulkan_vertex.h>

namespace duk::rhi {

VkFormat convert_vertex_attribute_format(VertexInput::Format format) {
    VkFormat converted;
    switch (format) {
        case VertexInput::Format::UNDEFINED:
            converted = VK_FORMAT_UNDEFINED;
            break;
        case VertexInput::Format::INT8:
            converted = VK_FORMAT_R8_SINT;
            break;
        case VertexInput::Format::UINT8:
            converted = VK_FORMAT_R8_UINT;
            break;
        case VertexInput::Format::INT16:
            converted = VK_FORMAT_R16_SINT;
            break;
        case VertexInput::Format::UINT16:
            converted = VK_FORMAT_R16_UINT;
            break;
        case VertexInput::Format::INT32:
            converted = VK_FORMAT_R32_SINT;
            break;
        case VertexInput::Format::UINT32:
            converted = VK_FORMAT_R32_UINT;
            break;
        case VertexInput::Format::FLOAT32:
            converted = VK_FORMAT_R32_SFLOAT;
            break;
        case VertexInput::Format::VEC2:
            converted = VK_FORMAT_R32G32_SFLOAT;
            break;
        case VertexInput::Format::VEC3:
            converted = VK_FORMAT_R32G32B32_SFLOAT;
            break;
        case VertexInput::Format::VEC4:
            converted = VK_FORMAT_R32G32B32A32_SFLOAT;
            break;
    }
    return converted;
}

}// namespace duk::rhi