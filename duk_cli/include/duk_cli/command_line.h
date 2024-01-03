//
// Created by rov on 12/15/2023.
//

#ifndef DUK_CLI_PARSER_H
#define DUK_CLI_PARSER_H

#include <duk_macros/macros.h>

#include <string>
#include <vector>

namespace duk::cli {

enum class Command {
    UNKNOWN,
    JSON_PARSER_GENERATOR
};

class CommandLine {
public:

    CommandLine(int argc, const char* argv[]);

    DUK_NO_DISCARD Command command() const;

    DUK_NO_DISCARD const std::string& input_filepath() const;

    DUK_NO_DISCARD const std::string& output_filepath() const;

    DUK_NO_DISCARD const std::string& output_namespace() const;

    DUK_NO_DISCARD const std::vector<std::string>& additional_includes() const;

private:
    Command m_command;
    std::string m_inputFilepath;
    std::string m_outputFilepath;
    std::string m_outputNamespace;
    std::vector<std::string> m_additionalIncludes;
};

}

#endif //DUK_CLI_PARSER_H
