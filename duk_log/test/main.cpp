#include <duk_log/logger.h>
#include <duk_log/log.h>
#include <duk_log/sink_cout.h>

int main() {
    //Simple usage of Duk Log for basic logging messages
    duk::log::warn("This is a simple warn log message");
    duk::log::debug("This is a simple debug log message");
    
    //Adding a new logger to the default log sink of the engine
    duk::log::Logger* logger;
    logger = duk::log::add_logger(std::make_unique<duk::log::Logger>(duk::log::DEBUG));
    logger->print(duk::log::DEBUG, "A debug log message from logger");

    
    //--------Advanced logging with separate logger and sink, and using wait functions--------

    
    //Create a new logger for specific logging separated from the engine logs already created 100% independent
    auto loggerWait = std::make_unique<duk::log::Logger>(duk::log::DEBUG);

    //Create a new log sink (output destination) for advanced logging
    auto sinkCout = std::make_unique<duk::log::SinkCout>(duk::log::INFO);

    //Flush a log message to the new sink
    sinkCout->flush(duk::log::INFO, "A info message from a new sink");

    //Now this log is connected with this sink, every time this logger prints a message, this sink will flush
    sinkCout->flush_from(*loggerWait);
    
    //Print a debug log message from the logger and wait for it to complete
    loggerWait->print(duk::log::DEBUG, "A debug log message from logger with wait method after being added to a new sink").wait();
    
    return 0;
}
