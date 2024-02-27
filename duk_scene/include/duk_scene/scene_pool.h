/// 23/02/2024
/// scene_pool.h

#ifndef DUK_SCENE_SCENE_POOL_H
#define DUK_SCENE_SCENE_POOL_H

#include <duk_resource/pool.h>
#include <duk_scene/scene.h>

namespace duk::scene {

class ScenePool : public duk::resource::PoolT<SceneResource> {};

}// namespace duk::scene

#endif// DUK_SCENE_SCENE_POOL_H
