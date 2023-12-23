//
// Created by rov on 12/15/2023.
//

#ifndef DUK_CLI_PARSER_H
#define DUK_CLI_PARSER_H

#include <duk_macros/macros.h>

#include <string>

namespace duk::cli {

class Parser {
public:

    Parser(int argc, const char* argv[]);

    DUK_NO_DISCARD const std::string& filepath() const;


private:
    std::string m_filepath;
};

}

#endif //DUK_CLI_PARSER_H
