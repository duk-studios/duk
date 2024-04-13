//
// Created by rov on 12/7/2023.
//

#ifndef DUK_ENGINE_SCENE_HANDLER_H
#define DUK_ENGINE_SCENE_HANDLER_H

#include <duk_engine/scene/scene_pool.h>

#include <duk_resource/handler.h>

#include <filesystem>

namespace duk::engine {

class SceneHandler : public duk::resource::ResourceHandlerT<ScenePool> {
public:
    SceneHandler();

protected:
    void load(ScenePool* pool, const resource::Id& id, const std::filesystem::path& path) override;
};

}// namespace duk::engine

#endif//DUK_ENGINE_SCENE_HANDLER_H
