//
// Created by Ricardo on 24/05/2024.
//

#include <duk_resource/handle.h>

struct Foo {
    float a;
};

struct Boo {
    bool a;
};

int main() {
    duk::resource::Handle<Foo> foo = std::make_shared<Foo>();

    duk::resource::Handle<void> voidFoo = foo;

    duk::resource::Handle<Foo> foo2 = voidFoo.as<Foo>();

    return 0;
}
