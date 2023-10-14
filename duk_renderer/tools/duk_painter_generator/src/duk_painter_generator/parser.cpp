/// 14/10/2023
/// parser.cpp

#include <duk_painter_generator/parser.h>

#include <cxxopts.hpp>
#include <iostream>

namespace duk::painter_generator {

Parser::Parser(int argc, char* argv[]) {

    using duk::rhi::Shader;

    cxxopts::Options options("duk_painter_generator", "Painter generator for duk_renderer");

    options.add_options()
            ("v, vert", "Vertex shader path", cxxopts::value<std::string>())
            ("f, frag", "Fragment shader path", cxxopts::value<std::string>())
            ("g, geom", "Geometry shader path", cxxopts::value<std::string>())
            ("e, tese", "Tesselation Evaluation shader path", cxxopts::value<std::string>())
            ("t, tesc", "Tesselation Control shader path", cxxopts::value<std::string>())
            ("c, comp", "Compute shader path", cxxopts::value<std::string>())
            ("n, name", "Output name prefix", cxxopts::value<std::string>())
            ("d, directory", "Output directory", cxxopts::value<std::string>())
            ;

    cxxopts::ParseResult result;
    try {
        result = options.parse(argc, argv);

        m_outputRootDir = result["directory"].as<std::string>();
        m_outputName = result["name"].as<std::string>();

        if (result.count("vert")) {
            m_inputSpvPaths[Shader::Module::VERTEX] = result["vert"].as<std::string>();
        }
        if (result.count("frag")) {
            m_inputSpvPaths[Shader::Module::FRAGMENT] = result["frag"].as<std::string>();
        }
        if (result.count("geom")) {
            m_inputSpvPaths[Shader::Module::GEOMETRY] = result["geom"].as<std::string>();
        }
        if (result.count("tesc")) {
            m_inputSpvPaths[Shader::Module::TESSELLATION_CONTROL] = result["tesc"].as<std::string>();
        }
        if (result.count("tese")) {
            m_inputSpvPaths[Shader::Module::TESSELLATION_EVALUATION] = result["tese"].as<std::string>();
        }
        if (result.count("comp")) {
            m_inputSpvPaths[Shader::Module::COMPUTE] = result["comp"].as<std::string>();
        }

        if (m_inputSpvPaths.empty()) {
            throw std::invalid_argument("no SPIR-V sources provided");
        }
    }
    catch (const std::exception& e) {
        std::cout << options.help() << std::endl;
        throw;
    }
}

Parser::~Parser() = default;

const std::string& Parser::output_root_dir() const {
    return m_outputRootDir;
}

const std::string& Parser::output_name() const {
    return m_outputName;
}

const Parser::ShaderPaths& Parser::input_spv_paths() const {
    return m_inputSpvPaths;
}

}