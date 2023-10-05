/// 30/09/2023
/// sort.cpp

#include <duk_renderer/sort.h>

namespace duk::renderer {

SortKey SortKey::calculate(const scene::Component<MeshDrawing>& meshPainter) {
    SortKey::Flags flags = {};
    flags.painterValue = reinterpret_cast<std::intptr_t>(meshPainter->painter);
    flags.paletteValue = reinterpret_cast<std::intptr_t>(meshPainter->palette);
    flags.meshValue = reinterpret_cast<std::intptr_t>(meshPainter->mesh);
    return SortKey{flags};
}

}