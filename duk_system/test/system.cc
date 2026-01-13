//
// Created by Ricardo on 04/07/2024.
//

#include <duk_system/system.h>
#include <duk_objects/objects.h>
#include <duk_tools/globals.h>
#include <duk_serial/json.h>
#include <catch2/catch_test_macros.hpp>
#include <sstream>

// Test system for verifying lifecycle and functionality
class TestSystem : public duk::system::System {
public:
    using System::attach;

    int attach_count = 0;
    int enter_count = 0;
    int update_count = 0;
    int exit_count = 0;

    void attach() override {
        attach_count++;
    }

    void enter() override {
        enter_count++;
    }

    void update() override {
        update_count++;
    }

    void exit() override {
        exit_count++;
    }
};

struct TestResource {
    int data = 0;
};

// Another test system
class AnotherTestSystem : public duk::system::System {
public:
    int update_count = 0;

    void update() override {
        update_count++;
    }

    int serializedValue;
    duk::resource::Handle<TestResource> resource;
};

// Test component for system interaction
struct TestComponent {
    int value = 0;
    std::string name;

    bool operator==(const TestComponent& other) const {
        return value == other.value && name == other.name;
    }
};

namespace duk::type {
// clang-format off
template<>
struct Type<TestComponent> : Class<TestComponent,
    Member<"value", &TestComponent::value>,
    Member<"name", &TestComponent::name>> {
};

template<>
struct Type<AnotherTestSystem> : Class<AnotherTestSystem,
    Member<"serializedValue", &AnotherTestSystem::serializedValue>,
    Member<"resource", &AnotherTestSystem::resource>> {
};
// clang-format on
} // namespace duk::type

TEST_CASE("Basic System usage and management", "[system]") {
    using namespace duk::system;

    duk::tools::Globals globals;
    duk::objects::Objects objects;
    duk::objects::ComponentEventDispatcher componentEventDispatcher;
    SystemEventDispatcher systemEventDispatcher(componentEventDispatcher);

    SECTION("System lifecycle management") {
        auto system = std::make_unique<TestSystem>();
        auto* testSys = system.get();
        testSys->attach(globals, objects, systemEventDispatcher);

        CHECK(testSys->globals() == &globals);
        CHECK(testSys->objects() == &objects);
        CHECK(testSys->attach_count == 1);

        testSys->enter();
        CHECK(testSys->enter_count == 1);

        testSys->update();
        CHECK(testSys->update_count == 1);

        testSys->exit();
        CHECK(testSys->exit_count == 1);
    }

    SECTION("Systems container management") {
        duk::system::register_system<TestSystem>();
        duk::system::register_system<AnotherTestSystem>();
        Systems systems;
        systems.attach(globals, objects, systemEventDispatcher);

        auto* testSys = systems.add<TestSystem>(0);
        REQUIRE(testSys != nullptr);
        CHECK(systems.get<TestSystem>() == testSys);

        auto* anotherSys = systems.add<AnotherTestSystem>(0);
        CHECK(systems.get<AnotherTestSystem>() == anotherSys);

        int count = 0;
        for (auto it = systems.begin(); it != systems.end(); ++it) {
            count++;
        }
        CHECK(count == 2);

        // Reset counters for lifecycle test
        testSys->enter_count = 0;
        testSys->update_count = 0;
        testSys->exit_count = 0;

        systems.enter(0);
        CHECK(testSys->enter_count == 1);

        systems.update(0);
        CHECK(testSys->update_count == 1);

        const uint32_t disableMask = (1 << 0);
        systems.update(disableMask);
        CHECK(testSys->update_count == 1);
        CHECK(anotherSys->update_count == 1);

        systems.exit(0);
        CHECK(testSys->exit_count == 1);
    }

    SECTION("System with component interactions") {

    }
}

TEST_CASE("System interaction with objects", "[system][object]") {
    using namespace duk::system;

    duk::tools::Globals globals;
    duk::objects::Objects objects;
    duk::objects::ComponentEventDispatcher componentEventDispatcher;
    SystemEventDispatcher systemEventDispatcher(componentEventDispatcher);
    duk::objects::register_component<TestComponent>();

    SECTION("System object creation and retrieval") {
        auto system = std::make_unique<TestSystem>();
        auto* testSys = system.get();
        testSys->attach(globals, objects, systemEventDispatcher);

        auto obj = testSys->create_object();
        CHECK(obj.valid());

        auto parent = testSys->create_object();
        REQUIRE(parent.valid());

        auto child = testSys->create_object(parent.id());
        REQUIRE(child.valid());

        auto retrieved = testSys->object(child.id());
        CHECK(retrieved.valid());
        CHECK(retrieved.id() == child.id());
    }

    SECTION("Objects can be iterated through systems") {
        auto system = std::make_unique<TestSystem>();
        auto* testSys = system.get();
        testSys->attach(globals, objects, systemEventDispatcher);

        auto obj1 = testSys->create_object();
        auto comp1 = obj1.add<TestComponent>();
        comp1->value = 10;

        auto obj2 = testSys->create_object();
        auto comp2 = obj2.add<TestComponent>();
        comp2->value = 20;

        auto results = testSys->all_objects_with<TestComponent>();

        objects.update(componentEventDispatcher);

        std::vector<int> values;
        for (auto obj : results) {
            auto c = obj.component<TestComponent>();
            REQUIRE(c.valid());
            values.push_back(c->value);
        }

        CHECK(values.size() == 2); // TestComp + comp1 + comp2
        CHECK(std::count(values.begin(), values.end(), 10) == 1);
        CHECK(std::count(values.begin(), values.end(), 20) == 1);
    }
}

TEST_CASE("System serialization and deserialization", "[system][json]") {
    using namespace duk::system;

    duk::system::register_system<TestSystem>();
    duk::system::register_system<AnotherTestSystem>();

    Systems input;
    input.add<TestSystem>(1);
    {
        auto testSys = input.add<AnotherTestSystem>(2);
        testSys->serializedValue = 42;
        testSys->resource = duk::resource::Handle<TestResource>(duk::resource::Id(7));
    }
    auto json = duk::serial::json_write(input);
    INFO("Json:" << json);
    auto output = duk::serial::json_read<Systems>(json);
    {
        auto testSys = output.get<TestSystem>();
        REQUIRE(testSys != nullptr);
        CHECK(output.group<TestSystem>() == 1);
    }
    {
        auto anotherSys = output.get<AnotherTestSystem>();
        REQUIRE(anotherSys != nullptr);
        CHECK(output.group<AnotherTestSystem>() == 2);
        CHECK(anotherSys->serializedValue == 42);
        CHECK(anotherSys->resource.id() == duk::resource::Id(7));
    }
}

TEST_CASE("System resource solving", "[system][resource]") {
    using namespace duk::system;
    using namespace duk::resource;

    duk::system::register_system<AnotherTestSystem>();

    Systems systems;
    auto anotherSys = systems.add<AnotherTestSystem>(0);
    anotherSys->resource = Handle<TestResource>(Id(3));

    Pools pools;
    auto testRes = std::make_shared<TestResource>();
    testRes->data = 123;
    pools.insert<TestResource>(Id(3), testRes);

    ReferenceSolver referenceSolver(pools);
    referenceSolver.solve(systems);

    REQUIRE(anotherSys->resource.valid());
    CHECK(anotherSys->resource->data == 123);
}
