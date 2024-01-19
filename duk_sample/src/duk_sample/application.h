/// 03/06/2023
/// application.h

#ifndef DUK_SAMPLE_APPLICATION_H
#define DUK_SAMPLE_APPLICATION_H

#include <duk_engine/engine.h>
#include <duk_renderer/brushes/mesh.h>
#include <duk_renderer/resources/materials/color/color_material.h>
#include <duk_renderer/resources/materials/phong/phong_material.h>

namespace duk::sample {

struct ApplicationCreateInfo {
    const char* name;
};

class Application {
public:

    explicit Application(const ApplicationCreateInfo& applicationCreateInfo);

    ~Application();

    void run();

private:
    std::unique_ptr<duk::engine::Engine> m_engine;
    std::unique_ptr<duk::scene::Scene> m_scene;
};

}

#endif // DUK_SAMPLE_APPLICATION_H

