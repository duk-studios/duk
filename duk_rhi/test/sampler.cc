//
// Created by Ricardo on 20/07/2023.
//


#include <duk_rhi/sampler.h>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Sampler serialization", "[rhi][json]") {
    SECTION("Filter mode serialization") {
        auto inputNearest = duk::rhi::Sampler::Filter::NEAREST;
        auto inputLinear = duk::rhi::Sampler::Filter::LINEAR;
        auto inputCubic = duk::rhi::Sampler::Filter::CUBIC;
        auto jsonNearest = duk::serial::json_write(inputNearest);
        auto jsonLinear = duk::serial::json_write(inputLinear);
        auto jsonCubic = duk::serial::json_write(inputCubic);
        INFO("Filter NEAREST json: " << jsonNearest);
        INFO("Filter LINEAR json: " << jsonLinear);
        INFO("Filter CUBIC json: " << jsonCubic);
        CHECK(jsonNearest == R"("nearest")");
        CHECK(jsonLinear == R"("linear")");
        CHECK(jsonCubic == R"("cubic")");
        auto outputNearest = duk::serial::json_read<duk::rhi::Sampler::Filter>(jsonNearest);
        auto outputLinear = duk::serial::json_read<duk::rhi::Sampler::Filter>(jsonLinear);
        auto outputCubic = duk::serial::json_read<duk::rhi::Sampler::Filter>(jsonCubic);
        CHECK(inputNearest == outputNearest);
        CHECK(inputLinear == outputLinear);
        CHECK(inputCubic == outputCubic);
    }
    SECTION("Wrap mode serialization") {
        auto inputClampToEdge = duk::rhi::Sampler::WrapMode::CLAMP_TO_EDGE;
        auto inputRepeat = duk::rhi::Sampler::WrapMode::REPEAT;
        auto inputMirroredRepeat = duk::rhi::Sampler::WrapMode::MIRRORED_REPEAT;
        auto inputClampToBorder = duk::rhi::Sampler::WrapMode::CLAMP_TO_BORDER;
        auto inputMirrorClampToEdge = duk::rhi::Sampler::WrapMode::MIRROR_CLAMP_TO_EDGE;
        auto jsonClampToEdge = duk::serial::json_write(inputClampToEdge);
        auto jsonRepeat = duk::serial::json_write(inputRepeat);
        auto jsonMirroredRepeat = duk::serial::json_write(inputMirroredRepeat);
        auto jsonClampToBorder = duk::serial::json_write(inputClampToBorder);
        auto jsonMirrorClampToEdge = duk::serial::json_write(inputMirrorClampToEdge);
        INFO("WrapMode CLAMP_TO_EDGE json: " << jsonClampToEdge);
        INFO("WrapMode REPEAT json: " << jsonRepeat);
        INFO("WrapMode MIRRORED_REPEAT json: " << jsonMirroredRepeat);
        INFO("WrapMode CLAMP_TO_BORDER json: " << jsonClampToBorder);
        INFO("WrapMode MIRROR_CLAMP_TO_EDGE json: " << jsonMirrorClampToEdge);
        CHECK(jsonClampToEdge == R"("clamp-edge")");
        CHECK(jsonRepeat == R"("repeat")");
        CHECK(jsonMirroredRepeat == R"("mirror-repeat")");
        CHECK(jsonClampToBorder == R"("clamp-border")");
        CHECK(jsonMirrorClampToEdge == R"("mirror-clamp-edge")");
        auto outputClampToEdge = duk::serial::json_read<duk::rhi::Sampler::WrapMode>(jsonClampToEdge);
        auto outputRepeat = duk::serial::json_read<duk::rhi::Sampler::WrapMode>(jsonRepeat);
        auto outputMirroredRepeat = duk::serial::json_read<duk::rhi::Sampler::WrapMode>(jsonMirroredRepeat);
        auto outputClampToBorder = duk::serial::json_read<duk::rhi::Sampler::WrapMode>(jsonClampToBorder);
        auto outputMirrorClampToEdge = duk::serial::json_read<duk::rhi::Sampler::WrapMode>(jsonMirrorClampToEdge);
        CHECK(inputClampToEdge == outputClampToEdge);
        CHECK(inputRepeat == outputRepeat);
        CHECK(inputMirroredRepeat == outputMirroredRepeat);
        CHECK(inputClampToBorder == outputClampToBorder);
        CHECK(inputMirrorClampToEdge == outputMirrorClampToEdge);
    }
    SECTION("Sampler roundtrip") {
        auto input = duk::rhi::Sampler{
            duk::rhi::Sampler::Filter::LINEAR,
            duk::rhi::Sampler::WrapMode::CLAMP_TO_EDGE
        };
        auto json = duk::serial::json_write(input);
        INFO("Sampler json: " << json);
        auto output = duk::serial::json_read<duk::rhi::Sampler>(json);
        CHECK(input == output);
    }
}