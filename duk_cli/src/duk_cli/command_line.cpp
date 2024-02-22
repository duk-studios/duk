//
// Created by rov on 12/15/2023.
//

#include <duk_cli/command_line.h>
#include <duk_cli/commands/json_command.h>

#include <cxxopts.hpp>
#include <duk_log/log.h>

namespace duk::cli {

namespace detail {

const char* kHelpMessage = R"(
duk
Usage:
  duk [COMMAND...]

 commands:
    json

)";

std::unique_ptr<Command> make_json_generate_command(int argc, const char* argv[]) {
    cxxopts::Options options("duk json generate");

    options.add_options()("s, source", "Source file to parse", cxxopts::value<std::string>())("o, output", "Output file with generated methods", cxxopts::value<std::string>())("n, namespace", "Namespace for generated methods", cxxopts::value<std::string>())("i, includes", "Additional includes for generated file", cxxopts::value<std::vector<std::string>>());

    auto result = options.parse(argc, argv);

    JsonCommandCreateInfo jsonCommandCreateInfo = {};
    jsonCommandCreateInfo.inputFilepath = result["source"].as<std::string>();
    jsonCommandCreateInfo.outputFilepath = result["output"].as<std::string>();
    jsonCommandCreateInfo.nameSpace = result["namespace"].as<std::string>();
    jsonCommandCreateInfo.additionalIncludes = result["includes"].as<std::vector<std::string>>();

    return std::make_unique<JsonCommand>(jsonCommandCreateInfo);
}

std::unique_ptr<Command> make_json_command(int argc, const char* argv[]) {
    if (argc < 1) {
        throw std::invalid_argument("insufficient arguments for json command");
    }

    std::string commandType = argv[1];

    if (commandType == "generate") {
        return make_json_generate_command(argc - 1, argv + 1);
    } else {
        throw std::invalid_argument("unknown json command: " + commandType);
    }
}

}// namespace detail

Command::~Command() = default;

CommandLine::CommandLine(int argc, const char* argv[]) {
    if (argc < 2) {
        throw std::invalid_argument("insufficient arguments");
    }

    std::string commandName = argv[1];

    if (commandName == "json") {
        m_command = detail::make_json_command(argc - 1, argv + 1);
    } else {
        throw std::invalid_argument("unknown command: " + commandName);
    }
}

Command* CommandLine::command() const {
    return m_command.get();
}

}// namespace duk::cli
