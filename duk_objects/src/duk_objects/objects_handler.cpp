//
// Created by Ricardo on 28/03/2024.
//

#include <duk_objects/objects_handler.h>

#include <duk_serial/json/serializer.h>

namespace duk::objects {

// without this the root object would be an array, which is not ideal
// if we need more members to this, a proper class might be a good idea
struct ObjectsWrapper {
    std::shared_ptr<Objects> objects;
};

}// namespace duk::objects

namespace duk::serial {

template<>
inline void from_json<duk::objects::ObjectsWrapper>(const rapidjson::Value& json, duk::objects::ObjectsWrapper& objectsWrapper) {
    from_json_member(json, "objects", *objectsWrapper.objects);
}

template<>
inline void to_json<duk::objects::ObjectsWrapper>(rapidjson::Document& document, rapidjson::Value& json, const duk::objects::ObjectsWrapper& objectsWrapper) {
    to_json_member(document, json, "objects", *objectsWrapper.objects);
}

}// namespace duk::serial

namespace duk::objects {

ObjectsHandler::ObjectsHandler()
    : TextResourceHandlerT("obj") {
}

bool ObjectsHandler::accepts(const std::string& extension) const {
    return extension == ".obj";
}

duk::resource::Handle<Objects> ObjectsHandler::load_from_text(ObjectsPool* pool, const resource::Id& id, const std::string_view& text) {
    ObjectsWrapper objectsWrapper = {};
    objectsWrapper.objects = std::make_shared<Objects>();

    duk::serial::read_json(text, objectsWrapper);

    return pool->insert(id, objectsWrapper.objects);
}

}// namespace duk::objects