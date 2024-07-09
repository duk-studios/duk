//
// Created by rov on 12/7/2023.
//

#ifndef DUK_ENGINE_SCENE_HANDLER_H
#define DUK_ENGINE_SCENE_HANDLER_H

#include <duk_engine/scene/scene.h>

#include <duk_resource/handler.h>

#include <filesystem>

namespace duk::engine {

class SceneHandler : public duk::resource::TextHandlerT<Scene> {
public:
    SceneHandler();

    bool accepts(const std::string& extension) const override;

protected:
    std::shared_ptr<Scene> load_from_text(duk::tools::Globals* globals, const std::string_view& text) override;
};

}// namespace duk::engine

#endif//DUK_ENGINE_SCENE_HANDLER_H
