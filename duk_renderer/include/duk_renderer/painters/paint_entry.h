/// 11/11/2023
/// paint_entry.h

#ifndef DUK_RENDERER_PAINT_ENTRY_H
#define DUK_RENDERER_PAINT_ENTRY_H

#include <duk_renderer/sort.h>

#include <duk_rhi/render_pass.h>

namespace duk::renderer {

class Brush;
class GlobalDescriptors;

struct PaintParams {
    uint32_t outputWidth;
    uint32_t outputHeight;
    duk::rhi::RenderPass* renderPass;
    Brush* brush;
    Palette* palette;
    GlobalDescriptors* globalDescriptors;
    size_t instanceCount;
    size_t firstInstance;
};

struct ObjectEntry {
    duk::scene::Object::Id objectId;
    Brush* brush{};
    Painter* painter{};
    Palette* palette{};
    SortKey sortKey{};
};

struct PaintEntry {
    PaintParams params;
    Painter* painter;
};

// specialization for duk_renderer/sort.h sort_key
template<>
inline SortKey SortKey::sort_key(const ObjectEntry& objectEntry) {
    return objectEntry.sortKey;
}

}
#endif // DUK_RENDERER_PAINT_ENTRY_H

