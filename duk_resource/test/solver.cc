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

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, Bar& bar) {
    solver->solve(bar.fooHandle);
}

}

namespace duk::type {

template<>
struct Type<Foo> : Class<Foo,
    Member<"fooValue", &Foo::fooValue>> {
};

template<>
struct Type<Bar> : Class<Bar,
    Member<"fooHandle", &Bar::fooHandle>> {
};

}

TEST_CASE("We can query dependencies and fix up resource references", "[resource]") {
    using namespace duk::resource;
    SECTION("Dependency solver") {
        DependencySolver solver;
        SECTION("Resource handles are tracked as dependencies") {
            Handle<Foo> fooHandle(Id(1));
            solver.solve(fooHandle);
            CHECK(solver.dependencies().size() == 1);
            CHECK(solver.dependencies().count(Id(1)) == 1);
        }
        SECTION("Types can be solved for resource handles") {
            Bar bar;
            bar.fooHandle = Handle<Foo>(Id(2));
            solver.solve(bar);
            CHECK(solver.dependencies().size() == 1);
            CHECK(solver.dependencies().count(Id(2)) == 1);
        }
    }
    SECTION("Reference solver") {
        Pools pools;
        ReferenceSolver solver(pools);
        SECTION("Resource handles can be fixed up") {
            auto fooResource = std::make_shared<Foo>();
            fooResource->fooValue = 99;
            auto fooHandle = pools.insert<Foo>(Id(3), fooResource);

            Bar bar;
            bar.fooHandle = Handle<Foo>(Id(3)); // initially just an id

            solver.solve(bar);

            REQUIRE(bar.fooHandle.valid());
            CHECK(bar.fooHandle->fooValue == 99);
        }
    }
}