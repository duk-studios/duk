//
// Created by rov on 10/4/2025.
//

#include <duk_animation/clip/property.h>
#include <duk_animation/clip/properties/sprite_property.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

namespace {
    struct TestEvaluator {
        using ValueType = float;

        [[maybe_unused]] static void evaluate(const duk::objects::Object&, const duk::animation::PropertyT<TestEvaluator>*, uint32_t) {
            // no-op for tests
        }

        [[maybe_unused]] static float interpolate(const float& from, const float& to, float progress) {
            return from + (to - from) * progress;
        }
    };
}

TEST_CASE("Properties can be used to store keyframes and evaluate them", "[animation]") {
    using Property = duk::animation::PropertyT<TestEvaluator>;

    SECTION("add values and query samples") {
        Property prop;
        prop.add_value(0, 1.0f);
        prop.add_value(10, 3.0f);

        REQUIRE(prop.samples() == 10);

        REQUIRE_THAT(prop.sample_at(0), Catch::Matchers::WithinAbs(1.0f, 1e-6f));
        REQUIRE_THAT(prop.sample_at(5), Catch::Matchers::WithinAbs(1.0f, 1e-6f));
        REQUIRE_THAT(prop.sample_at(10), Catch::Matchers::WithinAbs(3.0f, 1e-6f));
        REQUIRE_THAT(prop.sample_at(20), Catch::Matchers::WithinAbs(3.0f, 1e-6f));
    }

    SECTION("interpolation between keyframes") {
        Property prop;
        prop.add_value(0, 0.0f);
        prop.add_value(10, 10.0f);

        REQUIRE_THAT(prop.sample_interpolate(0), Catch::Matchers::WithinAbs(0.0f, 1e-6f));
        REQUIRE_THAT(prop.sample_interpolate(5), Catch::Matchers::WithinAbs(5.0f, 1e-6f));
        REQUIRE_THAT(prop.sample_interpolate(10), Catch::Matchers::WithinAbs(10.0f, 1e-6f));
        REQUIRE_THAT(prop.sample_interpolate(15), Catch::Matchers::WithinAbs(10.0f, 1e-6f));
    }

    SECTION("iteration over stored values") {
        Property prop;
        prop.add_value(1, 2.0f);
        prop.add_value(2, 3.0f);

        std::vector<TestEvaluator::ValueType> values(prop.begin(), prop.end());
        REQUIRE(values.size() == 2);
        REQUIRE_THAT(values[0], Catch::Matchers::WithinAbs(2.0f, 1e-6f));
        REQUIRE_THAT(values[1], Catch::Matchers::WithinAbs(3.0f, 1e-6f));
    }
}

TEST_CASE("Properties can be serialized with json", "[animation][json]") {
    using Property = duk::animation::PropertyT<TestEvaluator>;

    SECTION("Property round-trip") {
        Property input;
        input.add_value(0, 1.5f);
        input.add_value(5, 2.5f);
        input.add_value(10, 5.0f);

        const auto json = duk::serial::json_write(input);
        INFO("Property json: " << json);

        const auto output = duk::serial::json_read<Property>(json);

        CHECK(output.samples() == input.samples());
        CHECK(output.sample_at(0) == input.sample_at(0));
        CHECK(output.sample_at(5) == input.sample_at(5));
        CHECK(output.sample_at(10) == input.sample_at(10));
    }

    SECTION("Property through base class round-trip") {
        duk::animation::register_property<TestEvaluator>();
        std::unique_ptr<duk::animation::Property> input = std::make_unique<Property>();
        dynamic_cast<Property*>(input.get())->add_value(0, 4.0f);
        dynamic_cast<Property*>(input.get())->add_value(8, 8.0f);

        const auto json = duk::serial::json_write(input);
        INFO("Property json: " << json);

        std::unique_ptr<duk::animation::Property> output;
        duk::serial::json_read(json, output);

        auto* outputProp = dynamic_cast<Property*>(output.get());
        REQUIRE(outputProp != nullptr);

        CHECK(outputProp->samples() == dynamic_cast<Property*>(input.get())->samples());
        CHECK(outputProp->sample_at(0) == dynamic_cast<Property*>(input.get())->sample_at(0));
        CHECK(outputProp->sample_at(8) == dynamic_cast<Property*>(input.get())->sample_at(8));
    }

    SECTION("Sprite property round trip") {
        using SpritePropertyType = duk::animation::PropertyT<duk::animation::SpriteProperty>;

        duk::animation::register_property<duk::animation::SpriteProperty>();
        std::unique_ptr<duk::animation::Property> input = std::make_unique<SpritePropertyType>();

        // Add sprite values with different indices and resource IDs
        auto* inputSpriteProp = dynamic_cast<SpritePropertyType*>(input.get());
        REQUIRE(inputSpriteProp != nullptr);

        duk::animation::SpriteValue value1;
        value1.index = 0;
        value1.sprite = duk::renderer::SpriteResource(duk::resource::Id(100));
        inputSpriteProp->add_value(0, value1);

        duk::animation::SpriteValue value2;
        value2.index = 2;
        value2.sprite = duk::renderer::SpriteResource(duk::resource::Id(200));
        inputSpriteProp->add_value(10, value2);

        duk::animation::SpriteValue value3;
        value3.index = 5;
        value3.sprite = duk::renderer::SpriteResource(duk::resource::Id(300));
        inputSpriteProp->add_value(20, value3);

        const auto json = duk::serial::json_write(input);
        INFO("Sprite property json: " << json);

        std::unique_ptr<duk::animation::Property> output;
        duk::serial::json_read(json, output);

        auto* outputSpriteProp = dynamic_cast<SpritePropertyType*>(output.get());
        REQUIRE(outputSpriteProp != nullptr);

        CHECK(outputSpriteProp->samples() == inputSpriteProp->samples());

        // Check first value
        auto outValue1 = outputSpriteProp->sample_at(0);
        auto inValue1 = inputSpriteProp->sample_at(0);
        CHECK(outValue1.index == inValue1.index);
        CHECK(outValue1.sprite.id() == inValue1.sprite.id());

        // Check second value
        auto outValue2 = outputSpriteProp->sample_at(10);
        auto inValue2 = inputSpriteProp->sample_at(10);
        CHECK(outValue2.index == inValue2.index);
        CHECK(outValue2.sprite.id() == inValue2.sprite.id());

        // Check third value
        auto outValue3 = outputSpriteProp->sample_at(20);
        auto inValue3 = inputSpriteProp->sample_at(20);
        CHECK(outValue3.index == inValue3.index);
        CHECK(outValue3.sprite.id() == inValue3.sprite.id());
    }
}