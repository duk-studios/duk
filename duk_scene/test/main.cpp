//
// Created by Ricardo on 20/07/2023.
//

#include <duk_scene/scene.h>
#include <iostream>

struct TestData {
    int b;
};

using TestDataResource = duk::resource::ResourceT<TestData>;

struct ComponentTest {
    uint32_t a;
    uint32_t b;
    uint32_t c;

    ~ComponentTest() {
        std::cout << "Destructor ComponentTest" << std::endl;
    }
};

struct ComponentTest2 {
    TestDataResource res;
    uint32_t b;
    uint32_t c;

    ~ComponentTest2() {
        std::cout << "Destructor ComponentTest2" << std::endl;
    }
};

struct ComponentTest3 {
    uint32_t a;
    uint32_t b;
    uint32_t c;

    ~ComponentTest3() {
        std::cout << "Destructor ComponentTest3" << std::endl;
    }
};

class TestSystem : public duk::scene::System {
public:
    void enter() override {
        duk::log::info("TestSystem::enter");
    }

    void update() override {
        duk::log::info("TestSystem::update");
    }

    void exit() override {
        duk::log::info("TestSystem::exit");
    }
};

namespace duk::serial {

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, ComponentTest& componentTest) {
    visitor->visit_member(componentTest.a, MemberDescription("a"));
    visitor->visit_member(componentTest.b, MemberDescription("b"));
    visitor->visit_member(componentTest.c, MemberDescription("c"));
}

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, ComponentTest2& componentTest) {
    visitor->template visit_member<duk::resource::Resource>(componentTest.res, MemberDescription("res"));
    visitor->visit_member(componentTest.b, MemberDescription("b"));
    visitor->visit_member(componentTest.c, MemberDescription("c"));
}

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, ComponentTest3& componentTest) {
    visitor->visit_member(componentTest.a, MemberDescription("a"));
    visitor->visit_member(componentTest.b, MemberDescription("b"));
    visitor->visit_member(componentTest.c, MemberDescription("c"));
}

}// namespace duk::serial

int test_serialization(duk::scene::Scene* scene) {
    duk::scene::register_component<ComponentTest>();
    duk::scene::register_component<ComponentTest2>();
    duk::scene::register_component<ComponentTest3>();
    duk::scene::register_system<TestSystem>();

    duk::serial::JsonWriter writer;

    writer.visit(*scene);

    auto sceneJson = writer.print();

    duk::log::debug("Scene serialized: {}", sceneJson).wait();

    duk::serial::JsonReader reader(sceneJson.c_str());

    auto newScene = std::make_unique<duk::scene::Scene>();

    reader.visit(*newScene);

    writer.visit(*newScene);

    auto newSceneJson = writer.print();

    duk::log::debug("New scene serialized: {}", newSceneJson).wait();

    DUK_ASSERT(sceneJson == newSceneJson);

    return 0;
}

int main() {
    //The scene which is in use
    auto scene = std::make_unique<duk::scene::Scene>();
    scene->systems().add<TestSystem>();
    auto& objects = scene->objects();

    //Adding a new object to the scene
    auto obj0 = objects.add_object();

    //Adding a new component to this object
    obj0.add<ComponentTest>();

    //Adding new objects to the scene with new components
    auto obj1 = objects.add_object();
    obj1.add<ComponentTest2>();
    auto obj2 = objects.add_object();
    obj2.add<ComponentTest>();
    auto cmp2 = obj2.add<ComponentTest2>();
    cmp2->res = duk::resource::Id(320);
    obj2.add<ComponentTest3>();

    objects.add_object();
    objects.add_object();

    //Iterating through all the scene objects with specified components
    //The objects that have any component type like: ComponentTest, ComponentTest2, ComponentTest3, will be listed below
    for (auto object: objects.all_with<ComponentTest, ComponentTest2, ComponentTest3>()) {
        std::cout << "Id: " << object.id().index() << std::endl;

        auto [comp1, comp2, comp3] = object.components<ComponentTest, ComponentTest2, ComponentTest3>();
        comp1->a = 1;
        comp2->b = 2;
        comp3->c = 3;
    }

    //Removing the Component from obj with id
    obj0.remove<ComponentTest>();

    //Destroying object from scene
    obj1.destroy();

    test_serialization(scene.get());

    return 0;
}
