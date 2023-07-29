//
// Created by Ricardo on 20/07/2023.
//

#include <duk_scene/scene.h>
#include <iostream>

struct ComponentTest {
    int a;
    int b;
    int c;
};

struct ComponentTest2 {
    int a;
    int b;
    int c;
};

struct ComponentTest3 {
    int a;
    int b;
    int c;
};


int main() {

    duk::scene::Scene scene;

    auto obj0 = scene.add_object();
    obj0.add<ComponentTest>();

    auto obj1 = scene.add_object();
    obj1.add<ComponentTest2>();
    auto obj2 = scene.add_object();
    obj2.add<ComponentTest>();
    obj2.add<ComponentTest2>();
//    obj2.add<ComponentTest3>();

    scene.add_object();
    scene.add_object();

//    obj3.destroy();



    for (auto object : scene.objects_with_components<ComponentTest, ComponentTest2, ComponentTest3>()) {
        std::cout << "Id: " << object.id().index() << std::endl;
    }

    return 0;
}
