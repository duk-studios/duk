# duk_objects
- Defines a data-oriented objects composed of objects and components.
- Each object is simply an id which is used to access its components.

# Usage
```cpp
#include <duk_objects/objects.h>
#include <iostream>

//Creating components 
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

    //The objects which is in use
    auto objects = std::make_unique<duk::objects::Objects>();
    
    //Adding a new object to the objects
    auto obj0 = objects->add_object();

    //Adding a new component to this object
    obj0.add<ComponentTest>();

    //Adding new objects to the objects with new components
    auto obj1 = objects->add_object();
    obj1.add<ComponentTest2>();
    auto obj2 = objects->add_object();
    obj2.add<ComponentTest>();
    obj2.add<ComponentTest2>();
    obj2.add<ComponentTest3>();
    
    objects->add_object();
    objects->add_object();

    //Iterating through all the objects objects with specified components
    //The objects that have any component type like: ComponentTest, ComponentTest2, ComponentTest3, will be listed below
    for (auto object : objects->all_with<ComponentTest, ComponentTest2, ComponentTest3>()) {
        std::cout << "Id: " << object.id().index() << std::endl;
        
        auto [comp1, comp2, comp3] = object.components<ComponentTest, ComponentTest2, ComponentTest3>();
        comp1->a = 1;
        comp2->b = 2;
        comp3->c = 3;
    }

    //Removing the Component from obj with id
    objects->remove_component<ComponentTest>(obj0.id());

    //Destroying object from objects
    objects->destroy_object(obj0.id());

    return 0;
}
````