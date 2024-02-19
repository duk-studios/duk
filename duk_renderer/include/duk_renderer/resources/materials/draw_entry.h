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
    Material* material{};
    SortKey sortKey{};
};

struct MeshDrawEntry {
    DrawParams params;
    Material* material;
    Mesh* mesh;
    uint32_t instanceCount;
    uint32_t firstInstance;
};

struct SpriteEntry {
    duk::scene::Object::Id objectId;
    Material* material{};
    Sprite* sprite{};
    SortKey sortKey{};
};

struct SpriteDrawEntry {
    SpriteBrush* brush;
    Material* material;
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

