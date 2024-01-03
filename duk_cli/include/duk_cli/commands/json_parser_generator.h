//
// Created by rov on 12/16/2023.
//

#ifndef DUK_CLI_JSON_PARSER_GENERATOR_H
#define DUK_CLI_JSON_PARSER_GENERATOR_H

#include <duk_cli/command_line.h>

#include <vector>

namespace duk::cli {

struct ReflectedMemberDescription {
    std::string type;
    std::string name;
};

struct ReflectedClassDescription {
    std::vector<ReflectedMemberDescription> members;
    std::string name;
};

class Reflector {
public:

    explicit Reflector(const std::string& content);

    DUK_NO_DISCARD const std::vector<ReflectedClassDescription>& reflected_types() const;

private:
    std::vector<ReflectedClassDescription> m_reflectedClasses;
};

struct GenerateJsonParserInfo {
    std::string inputFilepath;
    std::string outputFilepath;
    std::string nameSpace;
    std::vector<std::string> additionalIncludes;
};

void generate_json_parser(const GenerateJsonParserInfo& generateJsonParserInfo);

}

#endif //DUK_CLI_JSON_PARSER_GENERATOR_H
