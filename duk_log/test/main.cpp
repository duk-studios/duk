#include <duk_log/log.h>
#include <duk_log/logger.h>
#include <duk_log/sinks/cout_sink.h>

int main() {
    //Simple usage of Duk Log for basic logging messages
    {
        duk::log::add_sink(std::make_unique<duk::log::CoutSink>("sink-0", duk::log::INFO));
        duk::log::warn("Hello warn!");

        duk::log::debug("Hello debug!");

        std::string argument = "argument";
        duk::log::debug("Hello {}!", argument);

        std::string argumentView = argument;
        duk::log::debug("Hello {} from a string_view!", argumentView);

        duk::log::info("Hello {} from a constant!", "my constant");
        //Adding a new logger to the default log sink of the engine
        auto logger = duk::log::add_logger("duk_log", duk::log::DEBUG);

        duk::log::debug(logger, "A debug log message from a new logger");

        std::string runtimeFmt = "This is a runtime string, value: {}";
        duk::log::info(runtimeFmt, 5);

        // wait for all logs
        duk::log::wait();
    }

    return 0;
}
