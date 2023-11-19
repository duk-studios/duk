//
// Created by Ricardo on 09/04/2023.
//

#include <duk_sample/application.h>

#include <duk_log/sink_std_console.h>

int main() {

    duk::log::SinkStdConsole consoleSink;

    duk::sample::ApplicationCreateInfo applicationCreateInfo = {};
    applicationCreateInfo.name = "Quacker";

    try {
        duk::sample::Application application(applicationCreateInfo);

        application.run();
    }
    catch (std::exception& e) {
        return 1;
    }
    return 0;
}