//
// Created by rov on 12/16/2023.
//

DUK_SERIALIZE_RESOURCE
struct Foo {

};

struct Boo {

};

DUK_SERIALIZE_RESOURCE
struct Bar {
    uint32_t a;
    float b;
    duk::pool::ResourceId resourceId;
};

