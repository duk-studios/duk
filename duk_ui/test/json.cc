//
// Created by rov on 10/4/2025.
//

#include <duk_ui/components/canvas.h>
#include <duk_ui/components/image.h>
#include <duk_ui/components/text.h>
#include <duk_ui/text/text_alignment.h>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("UI object serialization", "[ui][json]") {
    SECTION("Primitives") {
        SECTION("TextVertAlignment serialization") {
            auto inputTop = duk::ui::TextVertAlignment::TOP;
            auto inputMiddle = duk::ui::TextVertAlignment::MIDDLE;
            auto inputBottom = duk::ui::TextVertAlignment::BOTTOM;
            auto jsonTop = duk::serial::json_write(inputTop);
            auto jsonMiddle = duk::serial::json_write(inputMiddle);
            auto jsonBottom = duk::serial::json_write(inputBottom);
            INFO("TextVertAlignment TOP json: " << jsonTop);
            INFO("TextVertAlignment MIDDLE json: " << jsonMiddle);
            INFO("TextVertAlignment BOTTOM json: " << jsonBottom);
            CHECK(jsonTop == R"("top")");
            CHECK(jsonMiddle == R"("middle")");
            CHECK(jsonBottom == R"("bottom")");
            auto outputTop = duk::serial::json_read<duk::ui::TextVertAlignment>(jsonTop);
            auto outputMiddle = duk::serial::json_read<duk::ui::TextVertAlignment>(jsonMiddle);
            auto outputBottom = duk::serial::json_read<duk::ui::TextVertAlignment>(jsonBottom);
            CHECK(inputTop == outputTop);
            CHECK(inputMiddle == outputMiddle);
            CHECK(inputBottom == outputBottom);
        }
        SECTION("TextHoriAlignment serialization") {
            auto inputLeft = duk::ui::TextHoriAlignment::LEFT;
            auto inputMiddle = duk::ui::TextHoriAlignment::MIDDLE;
            auto inputRight = duk::ui::TextHoriAlignment::RIGHT;
            auto jsonLeft = duk::serial::json_write(inputLeft);
            auto jsonMiddle = duk::serial::json_write(inputMiddle);
            auto jsonRight = duk::serial::json_write(inputRight);
            INFO("TextHoriAlignment LEFT json: " << jsonLeft);
            INFO("TextHoriAlignment MIDDLE json: " << jsonMiddle);
            INFO("TextHoriAlignment RIGHT json: " << jsonRight);
            CHECK(jsonLeft == R"("left")");
            CHECK(jsonMiddle == R"("middle")");
            CHECK(jsonRight == R"("right")");
            auto outputLeft = duk::serial::json_read<duk::ui::TextHoriAlignment>(jsonLeft);
            auto outputMiddle = duk::serial::json_read<duk::ui::TextHoriAlignment>(jsonMiddle);
            auto outputRight = duk::serial::json_read<duk::ui::TextHoriAlignment>(jsonRight);
            CHECK(inputLeft == outputLeft);
            CHECK(inputMiddle == outputMiddle);
            CHECK(inputRight == outputRight);
        }
    }
    SECTION("Components") {
        using namespace duk::ui;
        SECTION("Canvas") {
            Canvas input = {.size = { 1280, 720 }};
            const auto json = duk::serial::json_write(input);
            INFO("Canvas json: " << json);
            const auto output = duk::serial::json_read<Canvas>(json);
            CHECK(input.size == output.size);
        }
        SECTION("CanvasTransform") {
            CanvasTransform input = {};
            input.position = {100.0f, 200.0f};
            input.anchor = {0.0f, 1.0f};
            input.pivot = {0.5f, 0.5f};
            input.size = {400.0f, 300.0f};
            input.scale = {1.0f, 1.0f};
            const auto json = duk::serial::json_write(input);
            INFO("CanvasTransform json: " << json);
            const auto output = duk::serial::json_read<CanvasTransform>(json);
            CHECK(input.position == output.position);
            CHECK(input.anchor == output.anchor);
            CHECK(input.pivot == output.pivot);
            CHECK(input.size == output.size);
            CHECK(input.scale == output.scale);
        }
        SECTION("Image") {
            Image input = {};
            input.image = duk::resource::Id(1234);
            input.sampler = duk::rhi::Sampler(duk::rhi::Sampler::Filter::LINEAR, duk::rhi::Sampler::WrapMode::CLAMP_TO_EDGE);
            input.color = glm::vec4(0.1f, 0.2f, 0.3f, 0.4f);
            input.keepAspectRatio = true;
            const auto json = duk::serial::json_write(input);
            INFO("Image json: " << json);
            const auto output = duk::serial::json_read<Image>(json);
            CHECK(input.image.id() == output.image.id());
            CHECK(input.sampler == output.sampler);
            CHECK(input.color == output.color);
            CHECK(input.keepAspectRatio == output.keepAspectRatio);
        }
        SECTION("Text") {
            Text input = {};
            input.text = "Hello, World!";
            input.font = duk::resource::Id(5678);
            input.fontSize = 24;
            input.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            input.horiAlignment = TextHoriAlignment::MIDDLE;
            input.vertAlignment = TextVertAlignment::TOP;
            input.dynamic = true;
            const auto json = duk::serial::json_write(input);
            INFO("Text json: " << json);
            const auto output = duk::serial::json_read<Text>(json);
            CHECK(input.text == output.text);
            CHECK(input.font.id() == output.font.id());
            CHECK(input.fontSize == output.fontSize);
            CHECK(input.color == output.color);
            CHECK(input.horiAlignment == output.horiAlignment);
            CHECK(input.vertAlignment == output.vertAlignment);
            CHECK(input.dynamic == output.dynamic);
        }
    }
}