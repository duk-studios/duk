//
// Created by Ricardo on 28/03/2024.
//

#include <duk_objects/objects_handler.h>

#include <duk_tools/file.h>

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
    : ResourceHandlerT("obj") {
}

bool ObjectsHandler::accepts(const std::string& extension) const {
    return extension == ".obj";
}

void ObjectsHandler::load(ObjectsPool* pool, const resource::Id& id, const std::filesystem::path& path) {
    auto content = duk::tools::File::load_text(path.string().c_str());

    ObjectsWrapper objectsWrapper = {};
    objectsWrapper.objects = std::make_shared<Objects>();

    duk::serial::read_json(content, objectsWrapper);

    pool->insert(id, objectsWrapper.objects);
}

}// namespace duk::objects