/// 23/02/2024
/// scene_pool.h

#ifndef DUK_ENGINE_SCENE_POOL_H
#define DUK_ENGINE_SCENE_POOL_H

#include <duk_engine/scene/scene.h>

#include <duk_resource/pool.h>

namespace duk::engine {

class ScenePool : public duk::resource::PoolT<SceneResource> {};

}// namespace duk::engine

#endif// DUK_ENGINE_SCENE_POOL_H
