/// 10/06/2023
/// resource.h

#ifndef DUK_RENDERER_RESOURCE_H
#define DUK_RENDERER_RESOURCE_H

#include <duk_events/event.h>
#include <duk_hash/hash.h>
#include <duk_macros/macros.h>

namespace duk::renderer {

/// Buffers and Images inherit from Resource, they can use this to announce when their contents changed,
/// then whoever is interested in this change may act accordingly (e.g. DescriptorSet, FrameBuffer)
class Resource {
public:
    using HashChangedEvent = duk::events::EventT<duk::hash::Hash>;

public:

    virtual ~Resource();

    DUK_NO_DISCARD HashChangedEvent& hash_changed_event();

protected:

    void hash_changed(duk::hash::Hash hash);

private:
    HashChangedEvent m_hashChangedEvent;
};

}

#endif // DUK_RENDERER_RESOURCE_H

