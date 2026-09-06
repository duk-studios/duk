/// shader_compiler.cc

#include <duk_rhi/shader_cache.h>
#include <duk_rhi/shader_compiler.h>

#include <catch2/catch_test_macros.hpp>

#include <cstring>
#include <unordered_map>

namespace {

constexpr std::string_view kMinimalVertexShader = R"glsl(
#version 450
void main() {
    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
}
)glsl";

constexpr std::string_view kMinimalFragmentShader = R"glsl(
#version 450
layout(location = 0) out vec4 outColor;
void main() {
    outColor = vec4(1.0, 0.0, 0.0, 1.0);
}
)glsl";

constexpr std::string_view kInvalidShader = R"glsl(
#version 450
this is not valid glsl at all !!!
)glsl";

constexpr uint32_t kSpvMagic = 0x07230203u;

bool has_spv_magic(const std::vector<uint8_t>& spirv) {
    if (spirv.size() < sizeof(uint32_t)) {
        return false;
    }
    uint32_t magic = 0;
    std::memcpy(&magic, spirv.data(), sizeof(magic));
    return magic == kSpvMagic;
}

duk::rhi::ShaderCompiler make_compiler() {
    duk::rhi::ShaderCompilerCreateInfo info{};
    info.api = duk::rhi::API::VULKAN;
    return duk::rhi::ShaderCompiler(info);
}

/// A cache implementation that counts load/store calls and stores data in memory.
class SpyCache : public duk::rhi::ShaderCache {
public:
    mutable int loadCallCount = 0;
    int storeCallCount = 0;

    std::expected<std::vector<uint8_t>, duk::rhi::ShaderCacheError> load(duk::hash::Hash hash) const noexcept override {
        ++loadCallCount;
        auto it = m_entries.find(hash);
        if (it == m_entries.end()) {
            return std::unexpected(duk::rhi::ShaderCacheError::NOT_FOUND);
        }
        return it->second;
    }

    void store(duk::hash::Hash hash, const std::vector<uint8_t>& spirv) noexcept override {
        ++storeCallCount;
        m_entries[hash] = spirv;
    }

private:
    std::unordered_map<duk::hash::Hash, std::vector<uint8_t>> m_entries;
};

}// anonymous namespace

TEST_CASE("ShaderCompiler basic compilation", "[rhi][shader_compiler]") {
    auto compiler = make_compiler();

    SECTION("vertex shader produces valid SPIR-V") {
        auto result = compiler.compile(kMinimalVertexShader, duk::rhi::ShaderModule::VERTEX, "test.vert");
        REQUIRE(result.has_value());
        CHECK(has_spv_magic(*result));
    }

    SECTION("fragment shader produces valid SPIR-V") {
        auto result = compiler.compile(kMinimalFragmentShader, duk::rhi::ShaderModule::FRAGMENT, "test.frag");
        REQUIRE(result.has_value());
        CHECK(has_spv_magic(*result));
    }

    SECTION("invalid GLSL returns a compilation error") {
        auto result = compiler.compile(kInvalidShader, duk::rhi::ShaderModule::VERTEX, "bad.vert");
        REQUIRE_FALSE(result.has_value());
        CHECK_FALSE(result.error().message.empty());
    }
}

TEST_CASE("ShaderCompiler cache interaction", "[rhi][shader_compiler]") {
    auto spy = std::make_shared<SpyCache>();
    duk::rhi::ShaderCompilerCreateInfo info{};
    info.api = duk::rhi::API::VULKAN;
    info.cache = spy;
    duk::rhi::ShaderCompiler compiler(info);

    SECTION("first compile misses cache and stores result") {
        auto result = compiler.compile(kMinimalVertexShader, duk::rhi::ShaderModule::VERTEX, "test.vert");
        REQUIRE(result.has_value());
        CHECK(spy->loadCallCount == 1);
        CHECK(spy->storeCallCount == 1);
    }

    SECTION("second compile hits the cache") {
        auto first = compiler.compile(kMinimalVertexShader, duk::rhi::ShaderModule::VERTEX, "test.vert");
        REQUIRE(first.has_value());

        auto second = compiler.compile(kMinimalVertexShader, duk::rhi::ShaderModule::VERTEX, "test.vert");
        REQUIRE(second.has_value());

        CHECK(spy->loadCallCount == 2);
        CHECK(spy->storeCallCount == 1);
        CHECK(*first == *second);
    }

    SECTION("different shader stages produce independent cache entries") {
        auto vert = compiler.compile(kMinimalVertexShader, duk::rhi::ShaderModule::VERTEX, "test.vert");
        auto frag = compiler.compile(kMinimalFragmentShader, duk::rhi::ShaderModule::FRAGMENT, "test.frag");
        REQUIRE(vert.has_value());
        REQUIRE(frag.has_value());
        CHECK(spy->storeCallCount == 2);
        CHECK(*vert != *frag);
    }

    SECTION("compilation error does not write to cache") {
        auto result = compiler.compile(kInvalidShader, duk::rhi::ShaderModule::VERTEX, "bad.vert");
        REQUIRE_FALSE(result.has_value());
        CHECK(spy->storeCallCount == 0);
    }
}

TEST_CASE("compile free function (ShaderCompiler overload)", "[rhi][shader_compiler]") {
    auto compiler = make_compiler();

    SECTION("vertex+fragment produces a source with both stages") {
        const std::unordered_map<duk::rhi::ShaderModule::Bits, std::string> sources = {
                {duk::rhi::ShaderModule::VERTEX,   std::string(kMinimalVertexShader)},
                {duk::rhi::ShaderModule::FRAGMENT, std::string(kMinimalFragmentShader)},
        };
        const auto source = duk::rhi::compile(compiler, sources).value();

        const auto mask = source.module_mask();
        CHECK(mask & duk::rhi::ShaderModule::VERTEX);
        CHECK(mask & duk::rhi::ShaderModule::FRAGMENT);
        CHECK(source.shader_modules().size() == 2);
        CHECK(source.hash() != 0);
    }
}

TEST_CASE("compile free function (ShaderCompilerCreateInfo overload)", "[rhi][shader_compiler]") {
    SECTION("compiles with default create info") {
        duk::rhi::ShaderCompilerCreateInfo info{};
        const std::unordered_map<duk::rhi::ShaderModule::Bits, std::string> sources = {
                {duk::rhi::ShaderModule::VERTEX,   std::string(kMinimalVertexShader)},
                {duk::rhi::ShaderModule::FRAGMENT, std::string(kMinimalFragmentShader)},
        };
        const auto source = duk::rhi::compile(info, sources).value();

        CHECK(source.module_mask() & duk::rhi::ShaderModule::VERTEX);
        CHECK(source.module_mask() & duk::rhi::ShaderModule::FRAGMENT);
        CHECK(source.hash() != 0);
    }

    SECTION("virtual includes are forwarded to the compiler") {
        duk::rhi::ShaderCompilerCreateInfo info{};
        info.virtualIncludes["color.glsl"] = "vec4 red() { return vec4(1.0, 0.0, 0.0, 1.0); }\n";

        const std::unordered_map<duk::rhi::ShaderModule::Bits, std::string> sources = {
                {duk::rhi::ShaderModule::VERTEX,   std::string(kMinimalVertexShader)},
                {duk::rhi::ShaderModule::FRAGMENT, R"glsl(
#version 450
#include "color.glsl"
layout(location = 0) out vec4 outColor;
void main() { outColor = red(); }
)glsl"},
        };
        const auto source = duk::rhi::compile(info, sources).value();

        CHECK(source.module_mask() & duk::rhi::ShaderModule::FRAGMENT);
    }
}

TEST_CASE("ShaderCompiler virtual includes", "[rhi][shader_compiler]") {
    duk::rhi::ShaderCompilerCreateInfo info{};
    info.api = duk::rhi::API::VULKAN;
    info.virtualIncludes["common.glsl"] = "vec4 red() { return vec4(1.0, 0.0, 0.0, 1.0); }\n";
    duk::rhi::ShaderCompiler compiler(info);

    constexpr std::string_view kShaderWithInclude = R"glsl(
#version 450
#include "common.glsl"
layout(location = 0) out vec4 outColor;
void main() {
    outColor = red();
}
)glsl";

    auto result = compiler.compile(kShaderWithInclude, duk::rhi::ShaderModule::FRAGMENT, "test_include.frag");
    REQUIRE(result.has_value());
    CHECK(has_spv_magic(*result));
}


