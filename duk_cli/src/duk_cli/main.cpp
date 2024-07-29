//
// Created by rov on 12/15/2023.
//

#include <duk_cli/command_line.h>

#include <duk_log/log.h>
#include <duk_log/cout_sink.h>

#include <duk_project/register_types.h>
#include <duk_platform/win32/platform_win_32.h>

int main(int argc, const char* argv[]) {
    // initialize platform specifics (like console virtualization on win32)
    auto platform = duk::platform::create_default_platform();

    try {
        duk::log::add_logger("duk", duk::log::INFO);
        duk::log::add_sink(std::make_unique<duk::log::CoutSink>("duk-cout", duk::log::INFO));

        duk::project::register_types();

        duk::cli::CommandLine commandLine(argc - 1, argv + 1);

        commandLine.command()->execute();
    } catch (const std::exception& e) {
        duk::log::fatal("exception caught: {}", e.what()).wait();
        return 1;
    }
    duk::log::wait();
    return 0;
}
