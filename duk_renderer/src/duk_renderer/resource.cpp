/// 10/06/2023
/// resource.cpp

#include <duk_renderer/resource.h>

namespace duk::renderer {

Resource::~Resource() = default;

Resource::HashChangedEvent& Resource::hash_changed_event() {
    return m_hashChangedEvent;
}

void Resource::hash_changed(duk::hash::Hash hash) {
    m_hashChangedEvent(hash);
}

}
