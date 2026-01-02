//
// Created by Ricardo on 20/07/2023.
//

#include <duk_resource/handle.h>
#include <duk_objects/objects.h>
#include <duk_objects/object_solver.h>
#include <duk_objects/events.h>
#include <duk_serial/json.h>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <ranges>
#include <sstream>

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

namespace duk::type {

template<>
struct Type<ComponentTest> : Class<ComponentTest,
    Member<"a", &ComponentTest::a>,
    Member<"b", &ComponentTest::b>,
    Member<"c", &ComponentTest::c>> {
};

template<>
struct Type<ComponentTest2> : Class<ComponentTest2,
    Member<"res", &ComponentTest2::res>,
    Member<"b", &ComponentTest2::b>,
    Member<"c", &ComponentTest2::c>> {
};

template<>
struct Type<ComponentTest3> : Class<ComponentTest3,
    Member<"a", &ComponentTest3::a>,
    Member<"b", &ComponentTest3::b>,
    Member<"c", &ComponentTest3::c>> {
};

} // namespace duk::type

TEST_CASE("Objects and Components Management", "[objects]") {
    // Register component types
    duk::objects::register_component<ComponentTest>();
    duk::objects::register_component<ComponentTest2>();
    duk::objects::register_component<ComponentTest3>();

    duk::event::Listener listener;
    duk::objects::ComponentEventDispatcher componentEventDispatcher;
    duk::objects::Objects objects;

    SECTION("Add and manage objects with components") {
        auto obj0 = objects.add_object();
        REQUIRE(obj0.valid());

        auto cmp = obj0.add<ComponentTest>();
        REQUIRE(cmp.valid());
        cmp->a = 10;
        cmp->b = 20;
        cmp->c = 30;
        CHECK(cmp->a == 10);
        CHECK(cmp->b == 20);
        CHECK(cmp->c == 30);
    }

    SECTION("Remove components from objects") {
        auto obj0 = objects.add_object();
        auto cmp_before = obj0.add<ComponentTest>();

        CHECK(cmp_before.valid());

        obj0.remove<ComponentTest>();

        // components are still valid until the next update
        CHECK(obj0.component<ComponentTest>().valid());

        // process component removal
        objects.update(componentEventDispatcher);

        auto cmp_after = obj0.component<ComponentTest>();
        CHECK(!cmp_after.valid());

    }
}

TEST_CASE("Serialization and Deserialization", "[objects][serialization]") {
    // Register component types
    duk::objects::register_component<ComponentTest>();
    duk::objects::register_component<ComponentTest2>();
    duk::objects::register_component<ComponentTest3>();

    duk::objects::ComponentEventDispatcher componentEventDispatcher;

    SECTION("Objects json roundtrip") {
        duk::objects::Objects objects;

        {
            auto obj = objects.add_object();
            auto comp = obj.add<ComponentTest>();
            comp->a = 42;
            comp->b = 43;
            comp->c = 44;
        }
        {
            auto obj = objects.add_object();
            auto comp = obj.add<ComponentTest2>();
            comp->res = duk::resource::Id(123);
            comp->b = 55;
            comp->c = 56;
        }
        {
            auto obj = objects.add_object();
            auto comp0 = obj.add<ComponentTest>();
            comp0->a = 7;
            comp0->b = 8;
            comp0->c = 9;
            auto comp1 = obj.add<ComponentTest2>();
            comp1->res = duk::resource::Id(320);
            comp1->b = 15;
            comp1->c = 16;
            auto comp2 = obj.add<ComponentTest3>();
            comp2->a = 21;
            comp2->b = 22;
            comp2->c = 23;
        }

        objects.add_object();
        objects.add_object();

        // Serialize to JSON
        std::ostringstream oss;
        auto json = duk::serial::json_write(objects);
        INFO("Objects json: " << json);

        // Deserialize from JSON
        duk::objects::Objects objects2;
        duk::serial::json_read(json, objects2);

        // Serialize again to verify roundtrip
        auto json2 = duk::serial::json_write(objects2);

        INFO("Objects json2: " << json2);

        CHECK(json == json2);
        CHECK(objects.count() == objects2.count());
    }
}
