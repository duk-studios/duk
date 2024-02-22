//
// Created by rov on 12/15/2023.
//

#ifndef DUK_CLI_PARSER_H
#define DUK_CLI_PARSER_H

#include <duk_macros/macros.h>

#include <memory>
#include <string>
#include <vector>

namespace duk::cli {

class Command {
public:
    virtual ~Command();

    virtual void execute() = 0;
};

class CommandLine {
public:
    CommandLine(int argc, const char* argv[]);

    DUK_NO_DISCARD Command* command() const;

private:
    std::unique_ptr<Command> m_command;
};

}// namespace duk::cli

#endif//DUK_CLI_PARSER_H
