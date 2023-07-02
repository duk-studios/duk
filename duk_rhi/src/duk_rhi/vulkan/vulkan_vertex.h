/// 21/05/2023
/// vulkan_vertex.h

#ifndef DUK_RHI_VULKAN_VERTEX_H
#define DUK_RHI_VULKAN_VERTEX_H

#include <duk_rhi/vertex_layout.h>
#include <duk_rhi/vulkan/vulkan_import.h>

namespace duk::rhi {

VkFormat convert_vertex_attribute_format(VertexAttribute::Format format);

}

#endif // DUK_RHI_VULKAN_VERTEX_H

