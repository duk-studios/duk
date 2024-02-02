/// 11/11/2023
/// paint_entry.h

#ifndef DUK_RENDERER_PAINT_ENTRY_H
#define DUK_RENDERER_PAINT_ENTRY_H

#include <duk_renderer/sort.h>

#include <duk_rhi/render_pass.h>

namespace duk::renderer {

class Brush;
class Mesh;
class Sprite;
class SpriteBrush;
class Material;
class MeshMaterial;
class SpriteMaterial;
class GlobalDescriptors;

struct DrawParams {
    uint32_t outputWidth;
    uint32_t outputHeight;
    duk::rhi::RenderPass* renderPass;
    GlobalDescriptors* globalDescriptors;
};

struct MeshEntry {
    duk::scene::Object::Id objectId;
    Mesh* mesh{};
    MeshMaterial* material{};
    SortKey sortKey{};
};

struct MeshDrawEntry {
    DrawParams params;
    MeshMaterial* material;
    Mesh* mesh;
    uint32_t instanceCount;
    uint32_t firstInstance;
};

struct SpriteEntry {
    duk::scene::Object::Id objectId;
    SpriteMaterial* material{};
    Sprite* sprite{};
    SortKey sortKey{};
    uint32_t materialIndex;
};

struct SpriteDrawEntry {
    SpriteBrush* brush;
    SpriteMaterial* material;
    uint32_t materialIndex;
    size_t instanceCount;
    size_t firstInstance;
};

// specialization for duk_renderer/sort.h sort_key
template<>
inline SortKey SortKey::sort_key(const MeshEntry& meshEntry) {
    return meshEntry.sortKey;
}

template<>
inline SortKey SortKey::sort_key(const SpriteEntry& spriteEntry) {
    return spriteEntry.sortKey;
}

}
#endif // DUK_RENDERER_PAINT_ENTRY_H

