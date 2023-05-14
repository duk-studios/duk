//
// Created by Ricardo on 04/02/2023.
//

#ifndef DUK_VULKAN_RENDERER_H
#define DUK_VULKAN_RENDERER_H

#include <duk_renderer/renderer.h>

namespace duk {

struct VulkanRendererCreateInfo {

};

class VulkanRenderer : public Renderer {
public:

    VulkanRenderer(const RendererCreateInfo& rendererCreateInfo, const VulkanRendererCreateInfo& vulkanRendererCreateInfo);

    void begin_frame() override;

    void end_frame() override;

private:

};

}

#endif //DUK_VULKAN_RENDERER_H
