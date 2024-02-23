/// 03/06/2023
/// application.h

#ifndef DUK_SAMPLE_APPLICATION_H
#define DUK_SAMPLE_APPLICATION_H

#include <duk_engine/engine.h>

namespace duk::sample {

struct ApplicationCreateInfo {
    std::string engineSettingsPath;
};

class Application {
public:
    explicit Application(const ApplicationCreateInfo& applicationCreateInfo);

    ~Application();

    void run();

private:
    std::unique_ptr<duk::engine::Engine> m_engine;
};

}// namespace duk::sample

#endif// DUK_SAMPLE_APPLICATION_H
