//
// Created by Ricardo on 09/04/2023.
//

#include <duk_sample/application.h>

#include <duk_log/log.h>

int main() {
    duk::sample::ApplicationCreateInfo applicationCreateInfo = {};
    applicationCreateInfo.engineSettingsPath = "./sample.duk";

    try {
        duk::sample::Application application(applicationCreateInfo);

        application.run();
    } catch (const std::exception& e) {
        duk::log::fatal("exception caught: {}", e.what());

        // guarantees that every log is printed
        duk::log::wait();
        return 1;
    }
    return 0;
}