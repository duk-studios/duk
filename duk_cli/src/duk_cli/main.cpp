//
// Created by rov on 12/15/2023.
//

#include <duk_cli/command_line.h>
#include <duk_cli/commands/json_command.h>

#include <duk_log/log.h>

int main(int argc, const char* argv[]) {
    try {
        duk::cli::CommandLine commandLine(argc - 1, argv + 1);

        commandLine.command()->execute();
    } catch (const std::exception& e) {
        duk::log::fatal("exception caught: {}", e.what()).wait();
        return 1;
    }
    duk::log::wait();
    return 0;
}