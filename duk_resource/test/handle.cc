//
// Created by rov on 10/4/2025.
//

#include <duk_resource/handle.h>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Resource handle serialization", "[json]") {
    using namespace duk::resource;
    SECTION("resource id roundtrip") {
        auto input = Id(12345);
        auto json = duk::serial::json_write(input);
        INFO("Json: " << json);
        auto output = duk::serial::json_read<Id>(json);
        CHECK(input == output);
    }

    SECTION("void resource handle roundtrip") {
        auto input = Handle<void>(Id(67890));
        auto json = duk::serial::json_write(input);
        INFO("Json: " << json);
        auto output = duk::serial::json_read<Handle<void>>(json);
        CHECK(input.id() == output.id());
    }

    SECTION("typed resource handle roundtrip") {
        struct Foo {};
        auto input = Handle<Foo>(Id(54321));
        auto json = duk::serial::json_write(input);
        INFO("Json: " << json);
        auto output = duk::serial::json_read<Handle<Foo>>(json);
        CHECK(input.id() == output.id());
    }
}

TEST_CASE("Resource handles can be used as smart pointers", "[resource]") {
    using namespace duk::resource;

    SECTION("Basic operations") {
        struct Bar {
            int value;
        };

        auto barPtr = std::make_shared<Bar>();
        barPtr->value = 42;
        Handle<Bar> handle(Id(1), barPtr);

        CHECK(handle->value == 42);
        CHECK((*handle).value == 42);
        Bar* rawPtr = handle.get();
        CHECK(rawPtr->value == 42);
        CHECK(handle.valid());
        CHECK(static_cast<bool>(handle));
        CHECK(handle.use_count() == 2); // one in handle, one in barPtr
    }

    SECTION("Type casting between handles") {
        struct Base {
            virtual ~Base() = default;
            int baseValue;
        };

        struct Derived : public Base {
            int derivedValue;
        };

        auto derivedPtr = std::make_shared<Derived>();
        derivedPtr->baseValue = 10;
        derivedPtr->derivedValue = 20;

        Handle<Derived> derivedHandle(Id(2), derivedPtr);
        Handle<Base> baseHandle = derivedHandle; // implicit conversion

        CHECK(baseHandle->baseValue == 10);

        Handle<Derived> castedHandle = baseHandle.as<Derived>();
        CHECK(castedHandle->derivedValue == 20);
    }

    SECTION("Type casting with invalid handle") {
        struct Base {
            virtual ~Base() = default;
        };

        struct Derived : public Base {};

        Handle<Base> invalidBaseHandle; // invalid handle
        Handle<Derived> castedHandle = invalidBaseHandle.as<Derived>();

        CHECK(!castedHandle.valid());
    }

    SECTION("Type casting to void handle") {
        struct Sample {
            int data;
        };

        auto samplePtr = std::make_shared<Sample>();
        samplePtr->data = 99;

        Handle<Sample> sampleHandle(Id(3), samplePtr);
        Handle<void> voidHandle = sampleHandle;

        CHECK(voidHandle.id() == sampleHandle.id());
        CHECK(voidHandle.use_count() == sampleHandle.use_count());
    }

}