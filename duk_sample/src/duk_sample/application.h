/// 03/06/2023
/// application.h

#ifndef DUK_SAMPLE_APPLICATION_H
#define DUK_SAMPLE_APPLICATION_H

#include <duk_engine/engine.h>
#include <duk_renderer/brushes/mesh.h>
#include <duk_renderer/materials/color/color_material.h>
#include <duk_renderer/materials/phong/phong_material.h>

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

    template<typename T>
    auto check_expected_result(T&& expectedResult) {
        if (!expectedResult) {
            auto errorDescription = expectedResult.error().description();
            m_engine->logger()->log_fatal() << "Unexpected result: " << errorDescription;
            std::terminate();
        }
        return std::move(expectedResult.value());
    }

private:
    std::unique_ptr<duk::engine::Engine> m_engine;
};

}

#endif // DUK_SAMPLE_APPLICATION_H

