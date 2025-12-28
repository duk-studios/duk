//
// Created by Ricardo on 28/03/2024.
//

#include <duk_objects/objects_handler.h>

#include <duk_type/describe_class.h>

#include <duk_serial/json.h>

namespace duk::objects {

// without this the root object would be an array, which is not ideal
// if we need more members to this, a proper class might be a good idea
struct ObjectsWrapper {
    std::shared_ptr<Objects> objects;
};

}// namespace duk::objects

template<>
struct duk::type::Type<duk::objects::ObjectsWrapper> : Class<objects::ObjectsWrapper,
            Member<"objects", &objects::ObjectsWrapper::objects>> {
};// namespace duk::type

namespace duk::objects {

ObjectsHandler::ObjectsHandler()
    : TextHandlerT("obj") {
}

bool ObjectsHandler::accepts(const std::string& extension) const {
    return extension == ".obj";
}

std::shared_ptr<Objects> ObjectsHandler::load_from_text(duk::tools::Globals* globals, const std::string_view& text) {
    ObjectsWrapper objectsWrapper = {};

    duk::serial::json_read(text, objectsWrapper);
    return objectsWrapper.objects;
}

}// namespace duk::objects