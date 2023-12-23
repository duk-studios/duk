//
// Created by rov on 12/15/2023.
//

#include <duk_cli/parser.h>
#include <duk_cli/commands/json_parser_generator.h>

#include <duk_log/log.h>

int main(int argc, const char* argv[]) {
    try {
        duk::cli::Parser parser(argc, argv);

        duk::cli::Reflector reflector(parser);
    }
    catch (const std::exception& e) {
        duk::log::fatal("exception caught: {}", e.what()).wait();
        return 1;
    }
    duk::log::wait();
    return 0;
}