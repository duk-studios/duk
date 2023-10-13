/// 12/09/2023
/// palette.h

#ifndef DUK_RENDERER_PALETTE_H
#define DUK_RENDERER_PALETTE_H

#include <duk_rhi/command/command_buffer.h>
#include <duk_scene/object.h>

namespace duk::renderer {

class GlobalDescriptors;

class Palette {
public:

    struct InsertInstanceParams {
        duk::scene::Object object;
    };

    virtual void insert_instance(const InsertInstanceParams& params) = 0;


    struct ApplyParams {
        GlobalDescriptors* globalDescriptors;
    };

    virtual void apply(duk::rhi::CommandBuffer* commandBuffer, const ApplyParams& params) = 0;

    virtual void clear();

};

}

#endif // DUK_RENDERER_PALETTE_H

