/// 11/11/2023
/// paint_entry.h

#ifndef DUK_RENDERER_PAINT_ENTRY_H
#define DUK_RENDERER_PAINT_ENTRY_H

#include <duk_renderer/sort.h>

#include <duk_rhi/render_pass.h>

namespace duk::renderer {

class Brush;
class Mesh;
class Material;
class MeshMaterial;
class GlobalDescriptors;

struct DrawParams {
    uint32_t outputWidth;
    uint32_t outputHeight;
    duk::rhi::RenderPass* renderPass;
    GlobalDescriptors* globalDescriptors;
};

struct ObjectEntry {
    duk::scene::Object::Id objectId;
    Brush* brush{};
    Material* material{};
    SortKey sortKey{};
};

struct MeshDrawEntry {
    DrawParams params;
    MeshMaterial* material;
    Mesh* mesh;
    uint32_t instanceCount;
    uint32_t firstInstance;
};

// specialization for duk_renderer/sort.h sort_key
template<>
inline SortKey SortKey::sort_key(const ObjectEntry& objectEntry) {
    return objectEntry.sortKey;
}

}
#endif // DUK_RENDERER_PAINT_ENTRY_H

