#include <duk_log/logger.h>
#include <duk_log/log.h>

#include "duk_log/sink_cout.h"

int main() {
    duk::log::warn("This is a simple warn log message").wait();
    duk::log::debug("This is a simple debug log message").wait();
    

    duk::log::Logger* logger;
    duk::log::SinkCout* sinkCout;
    
    logger = duk::log::add_logger(std::make_unique<duk::log::Logger>(duk::log::DEBUG));
    sinkCout = new duk::log::SinkCout(duk::log::INFO);
    logger->print(duk::log::DEBUG, "A debug log message from logger").wait();
    
    sinkCout->flush_from(*logger);
    
    return 0;
}
