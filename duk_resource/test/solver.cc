//
// Created by rov on 10/01/2026.
//

#include <duk_resource/handle.h>
#include <duk_resource/solver/dependency_solver.h>
#include <duk_resource/solver/reference_solver.h>
#include <catch2/catch_test_macros.hpp>

struct Foo {
    int fooValue;
};

struct Bar {
    duk::resource::Handle<Foo> fooHandle;
};

struct Baz {
    std::vector<duk::resource::Handle<Bar>> barHandles;
};

namespace duk::type {

// clang-format off
template<>
struct Type<Foo> : Class<Foo,
    Member<"fooValue", &Foo::fooValue>> {
};

template<>
struct Type<Bar> : Class<Bar,
    Member<"fooHandle", &Bar::fooHandle>> {
};

template<>
struct Type<Baz> : Class<Baz,
    Member<"barHandles", &Baz::barHandles>> {
};
// clang-format on

}

TEST_CASE("We can query dependencies and fix up resource references", "[resource]") {
    using namespace duk::resource;
    DependencySolver dependencySolver;
    Pools pools;
    ReferenceSolver referenceSolver(pools);
    SECTION("Resource handles are tracked as dependencies") {
        Handle<Foo> fooHandle(Id(1));
        dependencySolver.solve(fooHandle);
        CHECK(dependencySolver.dependencies().size() == 1);
        CHECK(dependencySolver.dependencies().count(Id(1)) == 1);
    }
    SECTION("Types can be solved for resource handles") {
        Bar bar;
        bar.fooHandle = Handle<Foo>(Id(2));
        dependencySolver.solve(bar);
        CHECK(dependencySolver.dependencies().size() == 1);
        CHECK(dependencySolver.dependencies().count(Id(2)) == 1);
    }
    SECTION("Resource handles can be fixed up") {
        auto fooResource = std::make_shared<Foo>();
        fooResource->fooValue = 99;
        auto fooHandle = pools.insert<Foo>(Id(3), fooResource);

        Bar bar;
        bar.fooHandle = Handle<Foo>(Id(3)); // initially just an id

        referenceSolver.solve(bar);

        REQUIRE(bar.fooHandle.valid());
        CHECK(bar.fooHandle->fooValue == 99);
    }
    SECTION("Containers of resource handles can be fixed up") {
        auto fooResource1 = std::make_shared<Foo>();
        fooResource1->fooValue = 11;
        auto fooHandle1 = pools.insert<Foo>(Id(4), fooResource1);

        auto fooResource2 = std::make_shared<Foo>();
        fooResource2->fooValue = 22;
        auto fooHandle2 = pools.insert<Foo>(Id(5), fooResource2);

        // Create Bars that reference the Foos
        auto barResource1 = std::make_shared<Bar>();
        barResource1->fooHandle = Handle<Foo>(Id(4));
        pools.insert<Bar>(Id(6), barResource1);

        auto barResource2 = std::make_shared<Bar>();
        barResource2->fooHandle = Handle<Foo>(Id(5));
        pools.insert<Bar>(Id(7), barResource2);

        // Now solve the Bars to fix up their Foo references
        referenceSolver.solve(barResource1);
        referenceSolver.solve(barResource2);

        Baz baz;
        baz.barHandles.push_back(Handle<Bar>(Id(6)));
        baz.barHandles.push_back(Handle<Bar>(Id(7)));

        referenceSolver.solve(baz);

        REQUIRE(baz.barHandles.size() == 2);
        REQUIRE(baz.barHandles[0].valid());
        REQUIRE(baz.barHandles[1].valid());
        REQUIRE(baz.barHandles[0]->fooHandle.valid());
        REQUIRE(baz.barHandles[1]->fooHandle.valid());
        CHECK(baz.barHandles[0]->fooHandle->fooValue == 11);
        CHECK(baz.barHandles[1]->fooHandle->fooValue == 22);
    }

}