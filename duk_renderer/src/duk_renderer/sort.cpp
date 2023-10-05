/// 30/09/2023
/// sort.cpp

#include <duk_renderer/sort.h>

namespace duk::renderer {

SortKey SortKey::calculate(const scene::Component<MeshDrawing>& meshDrawing) {
    SortKey::Flags flags = {};
    flags.painterValue = reinterpret_cast<std::intptr_t>(meshDrawing->painter);
    flags.paletteValue = reinterpret_cast<std::intptr_t>(meshDrawing->palette);
    flags.meshValue = reinterpret_cast<std::intptr_t>(meshDrawing->mesh);
    return SortKey{flags};
}

}