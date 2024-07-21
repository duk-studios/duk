//
// Created by Ricardo on 20/07/2023.
//

#include <duk_resource/handle.h>
#include <duk_objects/objects.h>
#include <duk_objects/object_solver.h>
#include <duk_objects/events.h>
#include <duk_serial/json/serializer.h>
#include <iostream>

struct TestData {
    int b;
};

using TestDataResource = duk::resource::Handle<TestData>;

struct ComponentTest {
    uint32_t a;
    uint32_t b;
    uint32_t c;

    ~ComponentTest() {
        duk::log::debug("Destructor ComponentTest");
    }
};

struct ComponentTest2 {
    TestDataResource res;
    uint32_t b;
    uint32_t c;

    ~ComponentTest2() {
        duk::log::debug("Destructor ComponentTest2");
    }
};

struct ComponentTest3 {
    uint32_t a;
    uint32_t b;
    uint32_t c;

    ~ComponentTest3() {
        duk::log::debug("Destructor ComponentTest3");
    }
};

namespace duk::serial {

template<>
inline void from_json<ComponentTest>(const rapidjson::Value& json, ComponentTest& componentTest) {
    from_json_member(json, "a", componentTest.a);
    from_json_member(json, "b", componentTest.b);
    from_json_member(json, "c", componentTest.c);
}

template<>
inline void to_json<ComponentTest>(rapidjson::Document& document, rapidjson::Value& json, const ComponentTest& componentTest) {
    to_json_member(document, json, "a", componentTest.a);
    to_json_member(document, json, "b", componentTest.b);
    to_json_member(document, json, "c", componentTest.c);
}

template<>
inline void from_json<ComponentTest2>(const rapidjson::Value& json, ComponentTest2& componentTest) {
    // from_json(json["res"], componentTest.res);
    from_json_member(json, "res", componentTest.res);
    from_json_member(json, "b", componentTest.b);
    from_json_member(json, "c", componentTest.c);
}

template<>
inline void to_json<ComponentTest2>(rapidjson::Document& document, rapidjson::Value& json, const ComponentTest2& componentTest) {
    to_json_member(document, json, "res", componentTest.res);
    to_json_member(document, json, "b", componentTest.b);
    to_json_member(document, json, "c", componentTest.c);
}

template<>
inline void from_json<ComponentTest3>(const rapidjson::Value& json, ComponentTest3& componentTest) {
    from_json_member(json, "a", componentTest.a);
    from_json_member(json, "b", componentTest.b);
    from_json_member(json, "c", componentTest.c);
}

template<>
inline void to_json<ComponentTest3>(rapidjson::Document& document, rapidjson::Value& json, const ComponentTest3& componentTest) {
    to_json_member(document, json, "a", componentTest.a);
    to_json_member(document, json, "b", componentTest.b);
    to_json_member(document, json, "c", componentTest.c);
}

}// namespace duk::serial

int main() {
    // register our component types
    duk::objects::register_component<ComponentTest>();
    duk::objects::register_component<ComponentTest2>();
    duk::objects::register_component<ComponentTest3>();

    duk::event::Listener listener;

    duk::objects::ComponentEventDispatcher componentEventDispatcher;

    //The objects which is in use
    duk::objects::Objects objects;

    //Adding a new object to the objects
    auto obj0 = objects.add_object();

    //Adding a new component to this object
    obj0.add<ComponentTest>();

    //Adding new objects to the objects with new components
    auto obj1 = objects.add_object();
    obj1.add<ComponentTest2>();
    auto obj2 = objects.add_object();
    obj2.add<ComponentTest>();
    auto cmp2 = obj2.add<ComponentTest2>();
    cmp2->res = duk::resource::Id(320);
    obj2.add<ComponentTest3>();

    objects.add_object();
    objects.add_object();

    struct CollisionEvent {
        duk::objects::Object object;
        duk::objects::Object other;
    };

    CollisionEvent collisionEvent = {};
    collisionEvent.object = obj1;
    collisionEvent.other = obj2;

    componentEventDispatcher.emit_object<CollisionEvent>(obj0, collisionEvent);

    //Removing the Component from obj with id
    obj0.remove<ComponentTest>();

    //Destroying object from objects
    obj1.destroy();

    // updating destroys all objects marked for destruction (via destroy)
    objects.update(componentEventDispatcher);

    //Iterating through all the objects with specified components
    //The objects that have any component type like: ComponentTest, ComponentTest2, ComponentTest3, will be listed below
    for (auto object: objects.all_with<ComponentTest, ComponentTest2, ComponentTest3>()) {
        std::cout << "Id: " << object.id().index() << std::endl;

        auto [comp1, comp2, comp3] = object.components<ComponentTest, ComponentTest2, ComponentTest3>();
        comp1->a = 1;
        comp2->b = 2;
        comp3->c = 3;
    }

    std::ostringstream oss;
    duk::serial::write_json(oss, objects, true);

    auto json = oss.str();

    duk::log::debug("objects json: {}", json);

    duk::objects::Objects objects2;

    duk::serial::read_json(json, objects2);

    std::ostringstream oss2;
    duk::serial::write_json(oss2, objects2, true);

    auto json2 = oss2.str();
    duk::log::debug("objects json2: {}", json2).wait();
    DUK_ASSERT(json == json2);

    return 0;
}
