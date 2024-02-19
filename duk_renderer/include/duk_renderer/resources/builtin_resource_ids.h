/// 19/02/2024
/// builtin_resource_ids.h

#ifndef DUK_RENDERER_BUILTIN_RESOURCE_IDS_H
#define DUK_RENDERER_BUILTIN_RESOURCE_IDS_H

#include <duk_resource/resource.h>

namespace duk::renderer {

// 1 - 1,000,000 - reserved for duk builtin resources

// 1 - 10,000 - images
static constexpr duk::resource::Id kWhiteImageId(1);
static constexpr duk::resource::Id kBlackImageId(2);

// 10,001 - 20,000 - mesh
static constexpr duk::resource::Id kQuadMeshId(10001);
static constexpr duk::resource::Id kCubeMeshId(10002);
static constexpr duk::resource::Id kSphereMeshId(10003);
static constexpr duk::resource::Id kConeMeshId(10004);

// 20,001 - 30,000 - sprites
static constexpr duk::resource::Id kWhiteSquareSpriteId(20001);

// 1,000,000 max builtin resource id
static constexpr duk::resource::Id kMaxBuiltinResourceId(1000000);

}

#endif // DUK_RENDERER_BUILTIN_RESOURCE_IDS_H

