//
// Created by Ricardo on 28/03/2024.
//

#ifndef DUK_OBJECTS_HANDLER_H
#define DUK_OBJECTS_HANDLER_H

#include <duk_objects/objects_pool.h>

#include <duk_resource/handler.h>

namespace duk::objects {

class ObjectsHandler : public duk::resource::ResourceHandlerT<ObjectsPool> {
public:
    ObjectsHandler();

    bool accepts(const std::string& extension) const override;

protected:
    void load(ObjectsPool* pool, const resource::Id& id, const std::filesystem::path& path) override;
};

}// namespace duk::objects

#endif//DUK_OBJECTS_HANDLER_H
