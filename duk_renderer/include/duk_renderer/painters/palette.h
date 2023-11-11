/// 12/09/2023
/// palette.h

#ifndef DUK_RENDERER_PALETTE_H
#define DUK_RENDERER_PALETTE_H

#include <duk_renderer/renderer.h>
#include <duk_rhi/command/command_buffer.h>
#include <duk_scene/object.h>

namespace duk::renderer {

class GlobalDescriptors;

class Palette {
public:

    virtual void clear_instances();

    struct InsertInstanceParams {
        duk::scene::Object object;
    };

    virtual void insert_instance(const InsertInstanceParams& params);

    virtual void flush_instances();

    struct ApplyParams {
        GlobalDescriptors* globalDescriptors;
    };

    virtual void apply(duk::rhi::CommandBuffer* commandBuffer, const ApplyParams& params) = 0;

};

}

#endif // DUK_RENDERER_PALETTE_H

