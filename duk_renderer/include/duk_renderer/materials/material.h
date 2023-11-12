/// 12/09/2023
/// material.h

#ifndef DUK_RENDERER_MATERIAL_H
#define DUK_RENDERER_MATERIAL_H

#include <duk_renderer/renderer.h>
#include <duk_renderer/materials/paint_entry.h>
#include <duk_renderer/materials/painter.h>

#include <duk_rhi/command/command_buffer.h>
#include <duk_scene/object.h>

namespace duk::renderer {

class GlobalDescriptors;

class Material {
protected:

    Material(Renderer* renderer, const duk::rhi::ShaderDataSource* shaderDataSource);

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

    virtual void paint(duk::rhi::CommandBuffer* commandBuffer, const PaintParams& params);

    DUK_NO_DISCARD Painter* painter();

    DUK_NO_DISCARD const Painter* painter() const;

private:
    Painter m_painter;

};

}

#endif // DUK_RENDERER_MATERIAL_H

