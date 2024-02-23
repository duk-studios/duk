//
// Created by rov on 1/19/2024.
//

#ifndef DUK_RENDERER_INSTANCE_BUFFER_H
#define DUK_RENDERER_INSTANCE_BUFFER_H

#include <duk_scene/objects.h>

namespace duk::renderer {

// Helper class to insert instances into a material
class InstanceBuffer {
public:
    virtual ~InstanceBuffer() = default;

    virtual void insert(const duk::scene::Object& object) = 0;

    virtual void clear() = 0;

    virtual void flush() = 0;
};

}// namespace duk::renderer

#endif//DUK_RENDERER_INSTANCE_BUFFER_H
