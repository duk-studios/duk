//
// Created by Ricardo on 01/05/2024.
//

#include <duk_runtime/application.h>

#include <duk_log/log.h>

int main() {
    try {
        duk::runtime::Application application;

        application.run();
    } catch (const std::exception& e) {
        duk::log::fatal("exception caught: {}", e.what());

        // guarantees that every log is printed
        duk::log::wait();
        return 1;
    }
    return 0;
}