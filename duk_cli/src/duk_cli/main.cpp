//
// Created by rov on 12/15/2023.
//

#include <duk_cli/command_line.h>
#include <duk_cli/commands/json_parser_generator.h>

#include <duk_log/log.h>

int main(int argc, const char* argv[]) {
    try {
        duk::cli::CommandLine commandLine(argc, argv);

        switch (commandLine.command()) {
            case duk::cli::Command::JSON_PARSER_GENERATOR:
            {
                duk::cli::GenerateJsonParserInfo generateJsonParserInfo = {};
                generateJsonParserInfo.inputFilepath = commandLine.input_filepath();
                generateJsonParserInfo.outputFilepath = commandLine.output_filepath();
                generateJsonParserInfo.nameSpace = commandLine.output_namespace();
                generateJsonParserInfo.additionalIncludes = commandLine.additional_includes();

                duk::cli::generate_json_parser(generateJsonParserInfo);
            }
                break;
            default:
                throw std::invalid_argument("unknown command type");
        }
    }
    catch (const std::exception& e) {
        duk::log::fatal("exception caught: {}", e.what()).wait();
        return 1;
    }
    duk::log::wait();
    return 0;
}