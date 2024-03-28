#ifndef DUK_RENDERER_FULLSCREEN_DESCRIPTORS_H
#define DUK_RENDERER_FULLSCREEN_DESCRIPTORS_H

#include <duk_renderer/material/fullscreen/fullscreen_shader_data_source.h>
#include <duk_renderer/material/material_descriptor_set.h>
#include <duk_rhi/rhi.h>

namespace duk::renderer {

class FullscreenDescriptors {
public:
    enum Bindings : uint32_t {
        uTexture = 0
    };

    static constexpr uint32_t kDescriptorCount = 1;
};

}// namespace duk::renderer

#endif// DUK_RENDERER_FULLSCREEN_DESCRIPTORS_H
