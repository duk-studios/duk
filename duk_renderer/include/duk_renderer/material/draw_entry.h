/// 11/11/2023
/// paint_entry.h

#ifndef DUK_RENDERER_DRAW_ENTRY_H
#define DUK_RENDERER_DRAW_ENTRY_H

#include <duk_renderer/sort.h>

namespace duk::renderer {

class Mesh;
class Material;
class GlobalDescriptors;

struct DrawParams {
    uint32_t outputWidth;
    uint32_t outputHeight;
    duk::rhi::RenderPass* renderPass;
    GlobalDescriptors* globalDescriptors;
};

struct ObjectEntry {
    duk::objects::Id objectId;
    Mesh* mesh{};
    Material* material{};
    SortKey sortKey{};
};

struct DrawEntry {
    Material* material;
    Mesh* mesh;
    uint32_t instanceCount;
    uint32_t firstInstance;
};

// specialization for duk_renderer/sort.h sort_key
template<>
inline SortKey SortKey::sort_key(const ObjectEntry& meshEntry) {
    return meshEntry.sortKey;
}

}// namespace duk::renderer
#endif// DUK_RENDERER_DRAW_ENTRY_H
