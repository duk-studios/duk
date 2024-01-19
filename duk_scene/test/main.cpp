//
// Created by Ricardo on 20/07/2023.
//

#include <duk_scene/scene.h>
#include <iostream>

struct ComponentTest {
    int a;
    int b;
    int c;

    ~ComponentTest() {
        std::cout << "Destructor ComponentTest" << std::endl;
    }
};

struct ComponentTest2 {
    int a;
    int b;
    int c;

    ~ComponentTest2() {
        std::cout << "Destructor ComponentTest2" << std::endl;
    }
};

struct ComponentTest3 {
    int a;
    int b;
    int c;
    
    ~ComponentTest3() {
        std::cout << "Destructor ComponentTest3" << std::endl;
    }
};


int main() {

    //The scene which is in use
    auto scene = std::make_unique<duk::scene::Scene>();
    
    //Adding a new object to the scene
    auto obj0 = scene->add_object();

    //Adding a new component to this object
    obj0.add<ComponentTest>();

    //Adding new objects to the scene with new components
    auto obj1 = scene->add_object();
    obj1.add<ComponentTest2>();
    auto obj2 = scene->add_object();
    obj2.add<ComponentTest>();
    obj2.add<ComponentTest2>();
    obj2.add<ComponentTest3>();
    
    scene->add_object();
    scene->add_object();

    //Iterating through all the scene objects with specified components
    //The objects that have any component type like: ComponentTest, ComponentTest2, ComponentTest3, will be listed below
    for (auto object : scene->objects_with_components<ComponentTest, ComponentTest2, ComponentTest3>()) {
        std::cout << "Id: " << object.id().index() << std::endl;
        
        auto [comp1, comp2, comp3] = object.components<ComponentTest, ComponentTest2, ComponentTest3>();
        comp1->a = 1;
        comp2->b = 2;
        comp3->c = 3;
    }

    //Removing the Component from obj with id
    scene->remove_component<ComponentTest>(obj0.id());

    //Destroying object from scene
    scene->destroy_object(obj0.id());

    return 0;
}
