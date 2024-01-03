//
// Created by rov on 12/15/2023.
//

#include <duk_cli/command_line.h>
#include <cxxopts.hpp>
#include <duk_log/log.h>

namespace duk::cli {

namespace detail {

Command parse_command(const std::string& command) {
    if (command == "json") {
        return Command::JSON_PARSER_GENERATOR;
    }
    return Command::UNKNOWN;
}

}

CommandLine::CommandLine(int argc, const char* argv[]) {
    cxxopts::Options options("duk", "duk cli");

    options.add_option("commands", "c", "command", "command to run", cxxopts::value<std::string>(), "specify which command should be executed");

    options.add_options("json")
            ("i, input", "File containing types to generate", cxxopts::value<std::string>())
            ("o, output", "Output file with generated content", cxxopts::value<std::string>())
            ("n, namespace", "Namespace for generated file", cxxopts::value<std::string>())
            ("e, includes", "Additional includes for generated file", cxxopts::value<std::vector<std::string>>())
            ;

    cxxopts::ParseResult result;

    duk::log::info("{}", options.help());

    result = options.parse(argc, argv);

    m_command = detail::parse_command(result["command"].as<std::string>());
    m_inputFilepath = result["input"].as<std::string>();
    m_outputFilepath = result["output"].as<std::string>();
    m_outputNamespace = result["namespace"].as<std::string>();
    m_additionalIncludes = result["includes"].as<std::vector<std::string>>();
}

Command CommandLine::command() const {
    return m_command;
}

const std::string& CommandLine::input_filepath() const {
    return m_inputFilepath;
}

const std::string& CommandLine::output_filepath() const {
    return m_outputFilepath;
}

const std::string& CommandLine::output_namespace() const {
    return m_outputNamespace;
}

const std::vector<std::string>& CommandLine::additional_includes() const {
    return m_additionalIncludes;
}

}
