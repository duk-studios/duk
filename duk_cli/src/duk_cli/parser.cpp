//
// Created by rov on 12/15/2023.
//

#include <duk_cli/parser.h>
#include <cxxopts.hpp>

namespace duk::cli {

Parser::Parser(int argc, const char** argv) {
    cxxopts::Options options("duk_painter_generator", "Painter generator for duk_renderer");

    options.add_options()
            ("f, file", "File containing types to generate", cxxopts::value<std::string>())
            ;

    cxxopts::ParseResult result;

    result = options.parse(argc, argv);

    m_filepath = result["file"].as<std::string>();

}

const std::string& Parser::filepath() const {
    return m_filepath;
}

}