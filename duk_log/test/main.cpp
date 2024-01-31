#include <duk_log/logger.h>
#include <duk_log/log.h>
#include <duk_log/sink_cout.h>

int main() {

    //Simple usage of Duk Log for basic logging messages
    {
        duk::log::warn("Hello warn!");

        duk::log::debug("Hello debug!");

        std::string argument = "argument";
        duk::log::debug("Hello {}!", argument);

        std::string_view argumentView = argument;
        duk::log::debug("Hello {} from a string_view!", argumentView);

        duk::log::info("Hello {} from a constant!", "my constant");

        //Adding a new logger to the default log sink of the engine
        auto logger = duk::log::add_logger(std::make_unique<duk::log::Logger>(duk::log::DEBUG));

        duk::log::debug(logger, "A debug log message from a new logger");

        // wait for all logs
        duk::log::wait();
    }

    
    //--------Advanced logging with separate logger and sink, and using wait functions--------

    {
        //Create a new logger for specific logging separated from the engine logs already created 100% independent
        auto logger = std::make_unique<duk::log::Logger>(duk::log::DEBUG);

        //Create a new log sink (output destination) for advanced logging
        auto sinkCout = std::make_unique<duk::log::SinkCout>(duk::log::INFO);

        //Flush a log message to the new sink
        sinkCout->flush(duk::log::INFO, "An info message sent directly into a new sink");

        //Now this log is connected with this sink, every time this logger prints a message, this sink will flush
        sinkCout->flush_from(*logger);

        //Print a debug log message from the logger and wait for it to complete
        logger->print(duk::log::DEBUG, "A debug log from a logger with wait method after being added to a new sink").wait();
    }
    
    return 0;
}
