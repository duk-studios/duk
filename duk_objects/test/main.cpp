//
// Created by Ricardo on 20/07/2023.
//

#include <duk_objects/objects.h>
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

int main() {

    // register our component types
    duk::objects::register_component<ComponentTest>();
    duk::objects::register_component<ComponentTest2>();
    duk::objects::register_component<ComponentTest3>();

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

    //Iterating through all the objects with specified components
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

    //Destroying object from objects
    obj1.destroy();

    // updating destroys all objects marked for destruction (via destroy)
    objects.update();

    return 0;
}
