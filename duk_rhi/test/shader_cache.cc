/// shader_cache.cc

#include <duk_rhi/shader_cache.h>

#include <catch2/catch_test_macros.hpp>

#include <cstdlib>
#include <filesystem>

namespace {

struct TempDir {
    std::filesystem::path path;

    TempDir() {
        path = std::filesystem::temp_directory_path() / ("duk_rhi_cache_test_" + std::to_string(std::rand()));
        std::filesystem::create_directories(path);
    }

    ~TempDir() {
        std::filesystem::remove_all(path);
    }
};

}// anonymous namespace

TEST_CASE("ShaderDiskCache", "[rhi][shader_cache]") {
    TempDir dir;
    duk::rhi::ShaderDiskCache cache(dir.path);

    SECTION("load returns NOT_FOUND for unknown hash") {
        auto result = cache.load(0xDEADBEEF);
        REQUIRE_FALSE(result.has_value());
        CHECK(result.error() == duk::rhi::ShaderCacheError::NOT_FOUND);
    }

    SECTION("store and load roundtrip") {
        const duk::hash::Hash hash = 0xCAFEBABE;
        const std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0xFF};
        cache.store(hash, data);
        auto result = cache.load(hash);
        REQUIRE(result.has_value());
        CHECK(*result == data);
    }

    SECTION("store creates intermediate directories") {
        auto nested = dir.path / "a" / "b" / "c";
        duk::rhi::ShaderDiskCache nestedCache(nested);
        const std::vector<uint8_t> data = {0xAA, 0xBB};
        nestedCache.store(0x1111, data);
        auto result = nestedCache.load(0x1111);
        REQUIRE(result.has_value());
        CHECK(*result == data);
    }

    SECTION("overwriting an entry replaces the stored data") {
        const duk::hash::Hash hash = 0x1234;
        cache.store(hash, {0x01});
        cache.store(hash, {0x02, 0x03});
        auto result = cache.load(hash);
        REQUIRE(result.has_value());
        CHECK(*result == std::vector<uint8_t>{0x02, 0x03});
    }

    SECTION("different hashes are stored independently") {
        const std::vector<uint8_t> dataA = {0xAA};
        const std::vector<uint8_t> dataB = {0xBB, 0xCC};
        cache.store(0x111, dataA);
        cache.store(0x222, dataB);
        CHECK(*cache.load(0x111) == dataA);
        CHECK(*cache.load(0x222) == dataB);
    }

    SECTION("store and load empty data") {
        const duk::hash::Hash hash = 0x5678;
        cache.store(hash, {});
        auto result = cache.load(hash);
        REQUIRE(result.has_value());
        CHECK(result->empty());
    }
}

