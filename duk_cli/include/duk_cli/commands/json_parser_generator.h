//
// Created by rov on 12/16/2023.
//

#ifndef DUK_CLI_JSON_PARSER_GENERATOR_H
#define DUK_CLI_JSON_PARSER_GENERATOR_H

#include <duk_cli/parser.h>

#include <vector>

namespace duk::cli {

enum class ReflectedType {
    INT8,
    UINT8,
    INT16,
    UINT16,
    INT32,
    UINT32,
    INT64,
    UINT64,
    FLOAT,
    DOUBLE,
    BOOL,
    STRING,
    RESOURCE_ID
};

std::string reflected_type_cpp_name(ReflectedType reflectedType);

struct ReflectedTypeDescription {
    ReflectedType type;
    std::string name;
};

class Reflector {
public:

    explicit Reflector(const Parser& parser);

    DUK_NO_DISCARD const std::vector<ReflectedTypeDescription>& reflected_types() const;

private:
    std::vector<ReflectedTypeDescription> m_reflectedTypes;
};

}

#endif //DUK_CLI_JSON_PARSER_GENERATOR_H
