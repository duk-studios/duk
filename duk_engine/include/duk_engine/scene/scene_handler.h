//
// Created by rov on 12/7/2023.
//

#ifndef DUK_ENGINE_SCENE_HANDLER_H
#define DUK_ENGINE_SCENE_HANDLER_H

#include <duk_engine/scene/scene_pool.h>

#include <duk_resource/handler.h>

#include <filesystem>

namespace duk::engine {

class SceneHandler : public duk::resource::TextHandlerT<ScenePool> {
public:
    SceneHandler();

    bool accepts(const std::string& extension) const override;

protected:
    duk::resource::Handle<Scene> load_from_text(ScenePool* pool, const resource::Id& id, const std::string_view& text) override;
};

}// namespace duk::engine

#endif//DUK_ENGINE_SCENE_HANDLER_H
