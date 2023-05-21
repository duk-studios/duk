/// 21/05/2023
/// vulkan_vertex.h

#ifndef DUK_RENDERER_VULKAN_VERTEX_H
#define DUK_RENDERER_VULKAN_VERTEX_H

#include <duk_renderer/mesh/vertex_layout.h>
#include <duk_renderer/vulkan/vulkan_import.h>

namespace duk::renderer::vk {

VkFormat convert_vertex_attribute_format(VertexAttribute::Format format);

}

#endif // DUK_RENDERER_VULKAN_VERTEX_H

