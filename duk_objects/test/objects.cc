//
// Created by Ricardo on 20/07/2023.
//

#include <duk_resource/handle.h>
#include <duk_objects/objects.h>
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

struct ComponentTest4 {
    duk::objects::Component<ComponentTest> comp1Ref;
};

struct EmptyComponent {
    // empty
};

namespace duk::type {
// clang-format off
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

template<>
struct Type<ComponentTest4> : Class<ComponentTest4,
    Member<"comp1Ref", &ComponentTest4::comp1Ref>> {
};

// clang-format on
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

    SECTION("Copying groups of objects into another") {
        duk::objects::Objects srcObjects;
        {
            auto obj0 = srcObjects.add_object();
            auto cmp0 = obj0.add<ComponentTest>();
            cmp0->a = 1;
            cmp0->b = 2;
            cmp0->c = 3;

            // hierarchy should be preserved after copy
            auto obj1 = srcObjects.add_object(obj0.id());
            auto cmp1 = obj1.add<ComponentTest2>();
            cmp1->b = 4;
            cmp1->c = 5;
        }

        // add some objects to destination
        {
            auto obj0 = objects.add_object();
            auto cmp0 = obj0.add<ComponentTest3>();
            cmp0->a = 6;
            cmp0->b = 7;
            cmp0->c = 8;
        }

        // copy srcObjects into objects
        objects.copy_objects(srcObjects);

        // update so the copied objects are available for search
        objects.update(componentEventDispatcher);

        REQUIRE(objects.count() == 3);
        auto [cmp0] = objects.first_of<ComponentTest>();
        REQUIRE(cmp0.valid());
        CHECK(cmp0->a == 1);
        CHECK(cmp0->b == 2);
        CHECK(cmp0->c == 3);
        auto [cmp1] = objects.first_of<ComponentTest2>();
        REQUIRE(cmp1.valid());
        CHECK(cmp1->b == 4);
        CHECK(cmp1->c == 5);
        auto cmp1Parent = cmp1.parent();
        CHECK(cmp1Parent.valid());
        CHECK(cmp1Parent.id() == cmp0.object().id());

        auto [cmp3] = objects.first_of<ComponentTest3>();
        REQUIRE(cmp3.valid());
        CHECK(cmp3->a == 6);
        CHECK(cmp3->b == 7);
        CHECK(cmp3->c == 8);
    }

    SECTION("Object hierarchy iteration - children_with and children_of") {
        // Create parent with children
        auto parent = objects.add_object();
        auto parent_cmp = parent.add<ComponentTest>();
        parent_cmp->a = 1;

        auto child1 = objects.add_object(parent.id());
        auto child1_cmp1 = child1.add<ComponentTest>();
        auto child1_cmp2 = child1.add<ComponentTest2>();
        child1_cmp1->a = 10;
        child1_cmp2->b = 100;

        auto child2 = objects.add_object(parent.id());
        auto child2_cmp = child2.add<ComponentTest>();
        child2_cmp->a = 11;

        auto child3 = objects.add_object(parent.id());
        child3.add<ComponentTest2>(); // No ComponentTest

        objects.update(componentEventDispatcher);

        // Test children_with single component
        auto children_with_test = objects.children_with<ComponentTest>(parent.id());
        int count_with_test = 0;
        for (auto obj : children_with_test) {
            count_with_test++;
            REQUIRE(obj.component<ComponentTest>().valid());
        }
        CHECK(count_with_test == 2);

        // Test children_with multiple components
        auto children_with_both = objects.children_with<ComponentTest, ComponentTest2>(parent.id());
        int count_with_both = 0;
        for (auto obj : children_with_both) {
            count_with_both++;
        }
        CHECK(count_with_both == 1);

        // Test children_of
        auto children_components = objects.children_of<ComponentTest>(parent.id());
        std::vector<uint32_t> a_values;
        for (auto [cmp] : children_components) {
            REQUIRE(cmp.valid());
            a_values.push_back(cmp->a);
        }
        REQUIRE(a_values.size() == 2);
        CHECK(a_values[0] == 10);
        CHECK(a_values[1] == 11);

        // Test children_of with multiple components
        auto children_multi = objects.children_of<ComponentTest, ComponentTest2>(parent.id());
        int count_multi = 0;
        for (auto [cmp1, cmp2] : children_multi) {
            count_multi++;
            CHECK(cmp1->a == 10);
            CHECK(cmp2->b == 100);
        }
        CHECK(count_multi == 1);
    }

    SECTION("Object hierarchy iteration - root_with and root_of") {
        // Create root and non-root objects
        auto root1 = objects.add_object();
        auto root1_cmp1 = root1.add<ComponentTest>();
        auto root1_cmp2 = root1.add<ComponentTest2>();
        root1_cmp1->a = 1;
        root1_cmp2->b = 100;

        auto root2 = objects.add_object();
        auto root2_cmp = root2.add<ComponentTest>();
        root2_cmp->a = 2;

        auto root3 = objects.add_object();
        root3.add<ComponentTest2>(); // No ComponentTest

        // Add child (should not appear in root queries)
        auto child = objects.add_object(root1.id());
        auto child_cmp = child.add<ComponentTest>();
        child_cmp->a = 10;

        objects.update(componentEventDispatcher);

        // Test root_with single component
        auto roots_with_test = objects.root_with<ComponentTest>();
        int count_root_test = 0;
        for (auto obj : roots_with_test) {
            count_root_test++;
            REQUIRE(obj.component<ComponentTest>().valid());
        }
        CHECK(count_root_test == 2);

        // Test root_with multiple components
        auto roots_with_both = objects.root_with<ComponentTest, ComponentTest2>();
        int count_root_both = 0;
        for (auto obj : roots_with_both) {
            count_root_both++;
        }
        CHECK(count_root_both == 1);

        // Test root_of
        auto roots_components = objects.root_of<ComponentTest>();
        std::vector<uint32_t> root_a_values;
        for (auto [cmp] : roots_components) {
            REQUIRE(cmp.valid());
            root_a_values.push_back(cmp->a);
        }
        REQUIRE(root_a_values.size() == 2);
        CHECK(root_a_values[0] == 1);
        CHECK(root_a_values[1] == 2);

        // Test root_of with multiple components
        auto roots_multi = objects.root_of<ComponentTest, ComponentTest2>();
        int count_root_multi = 0;
        for (auto [cmp1, cmp2] : roots_multi) {
            count_root_multi++;
            CHECK(cmp1->a == 1);
            CHECK(cmp2->b == 100);
        }
        CHECK(count_root_multi == 1);
    }

    SECTION("Multi-level hierarchy traversal") {
        // Create a complex hierarchy:
        // root1
        //   ├── child1
        //   │   ├── grandchild1
        //   │   └── grandchild2
        //   └── child2
        // root2
        //   └── child3

        auto root1 = objects.add_object();
        auto root1_cmp = root1.add<ComponentTest>();
        root1_cmp->a = 1;

        auto child1 = objects.add_object(root1.id());
        auto child1_cmp = child1.add<ComponentTest>();
        child1_cmp->a = 10;

        auto grandchild1 = objects.add_object(child1.id());
        auto grandchild1_cmp = grandchild1.add<ComponentTest>();
        grandchild1_cmp->a = 100;

        auto grandchild2 = objects.add_object(child1.id());
        auto grandchild2_cmp = grandchild2.add<ComponentTest>();
        grandchild2_cmp->a = 101;

        auto child2 = objects.add_object(root1.id());
        auto child2_cmp = child2.add<ComponentTest>();
        child2_cmp->a = 11;

        auto root2 = objects.add_object();
        auto root2_cmp = root2.add<ComponentTest>();
        root2_cmp->a = 2;

        auto child3 = objects.add_object(root2.id());
        auto child3_cmp = child3.add<ComponentTest>();
        child3_cmp->a = 20;

        objects.update(componentEventDispatcher);

        // Verify root_of returns only roots
        auto roots = objects.root_of<ComponentTest>();
        std::vector<uint32_t> root_a_values;
        for (auto [cmp] : roots) {
            root_a_values.push_back(cmp->a);
        }
        REQUIRE(root_a_values.size() == 2);
        CHECK(root_a_values[0] == 1);
        CHECK(root_a_values[1] == 2);

        // Verify children_of(root1) returns only direct children
        auto root1_children = objects.children_of<ComponentTest>(root1.id());
        std::vector<uint32_t> root1_child_values;
        for (auto [cmp] : root1_children) {
            root1_child_values.push_back(cmp->a);
        }
        REQUIRE(root1_child_values.size() == 2);
        CHECK(root1_child_values[0] == 10);
        CHECK(root1_child_values[1] == 11);

        // Verify children_of(child1) returns grandchildren
        auto child1_children = objects.children_of<ComponentTest>(child1.id());
        std::vector<uint32_t> child1_child_values;
        for (auto [cmp] : child1_children) {
            child1_child_values.push_back(cmp->a);
        }
        REQUIRE(child1_child_values.size() == 2);
        CHECK(child1_child_values[0] == 100);
        CHECK(child1_child_values[1] == 101);
    }
}

TEST_CASE("Objects serialization", "[objects][json]") {
    // Register component types
    duk::objects::register_component<ComponentTest>();
    duk::objects::register_component<ComponentTest2>();
    duk::objects::register_component<ComponentTest3>();
    duk::objects::register_component<ComponentTest4>();
    duk::objects::register_component<EmptyComponent>();

    duk::objects::ComponentEventDispatcher componentEventDispatcher;

    SECTION("Objects json roundtrip") {
        duk::objects::Objects objects;

        {
            auto obj = objects.add_object();
            auto comp = obj.add<ComponentTest>();
            comp->a = 42;
            comp->b = 43;
            comp->c = 44;
            [[maybe_unused]] auto a = obj.add<EmptyComponent>();

            auto obj2 = objects.add_object();
            auto comp2 = obj2.add<ComponentTest4>();
            comp2->comp1Ref = comp; // reference object 1 component

            auto obj3 = objects.add_object(obj.id()); // child of obj
            auto comp3 = obj3.add<ComponentTest2>();
            comp3->res = duk::resource::Id(123);
            comp3->b = 55;
            comp3->c = 56;
        }
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

        objects2.update(componentEventDispatcher); // update so we can iterate and search objects

        CHECK(json == json2);
        CHECK(objects.count() == objects2.count());
        auto[comp4] = objects2.first_of<ComponentTest4>();
        REQUIRE(comp4.valid());
        auto comp1Ref = comp4->comp1Ref;
        REQUIRE(comp1Ref.valid());
        CHECK(comp1Ref->a == 42);
        CHECK(comp1Ref->b == 43);
        CHECK(comp1Ref->c == 44);

        auto[comp1] = objects2.first_of<ComponentTest>();
        REQUIRE(comp1.valid());
        CHECK(comp1.component<EmptyComponent>().valid());
        CHECK(comp1->a == 42);
        CHECK(comp1->b == 43);
        CHECK(comp1->c == 44);
        auto[comp2] = objects2.first_of<ComponentTest2>();
        REQUIRE(comp2.valid());
        CHECK(comp2->res.id().value() == 123);
        CHECK(comp2->b == 55);
        CHECK(comp2->c == 56);
        CHECK(comp2.parent().id() == comp1.object().id());
    }
}

TEST_CASE("Component resources can be solved", "[objects][resource]") {
    // Register component types
    duk::objects::register_component<ComponentTest2>();

    duk::resource::Pools pools;
    duk::objects::ComponentEventDispatcher componentEventDispatcher;
    duk::objects::Objects objects;

    pools.insert(duk::resource::Id(1), std::make_shared<TestData>(TestData{ 99 }));

    // Create an object with a ComponentTest2 that has a resource handle with id 1
    auto obj = objects.add_object();
    auto comp = obj.add<ComponentTest2>();
    comp->res = duk::resource::Handle<TestData>(duk::resource::Id(1));
    comp->b = 10;
    comp->c = 20;

    // Solve resource references
    duk::resource::ReferenceSolver referenceSolver(pools);
    referenceSolver.solve(objects);

    // Verify that the resource handle in the component is correctly resolved
    REQUIRE(comp->res.valid());
    CHECK(comp->res->b == 99);
}
