/// 14/10/2023
/// parser.cpp

#include <duk_painter_generator/parser.h>

#include <cxxopts.hpp>
#include <iostream>

namespace duk::painter_generator {

Parser::Parser(int argc, char* argv[]) : m_printDebugInfo(false) {

    using duk::rhi::Shader;

    cxxopts::Options options("duk_painter_generator", "Painter generator for duk_renderer");

    options.add_options()
            ("v, vert", "Vertex shader path", cxxopts::value<std::string>())
            ("f, frag", "Fragment shader path", cxxopts::value<std::string>())
            ("g, geom", "Geometry shader path", cxxopts::value<std::string>())
            ("e, tese", "Tesselation Evaluation shader path", cxxopts::value<std::string>())
            ("t, tesc", "Tesselation Control shader path", cxxopts::value<std::string>())
            ("c, comp", "Compute shader path", cxxopts::value<std::string>())
            ("n, name", "Painter name", cxxopts::value<std::string>())
            ("s, src", "Source output directory", cxxopts::value<std::string>())
            ("i, inc", "Include output directory", cxxopts::value<std::string>())
            ("l, globals", "Global bindings name list", cxxopts::value<std::vector<std::string>>())
            ("d, debug", "Print debug information")
            ;

    cxxopts::ParseResult result;
    try {
        result = options.parse(argc, argv);

        m_outputSourceDirectory = result["src"].as<std::string>();
        m_outputIncludeDirectory = result["inc"].as<std::string>();
        m_outputPainterName = result["name"].as<std::string>();

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
        if (result.count("debug")) {
            m_printDebugInfo = true;
        }
        if (result.count("globals")) {
            const auto& globals = result["globals"].as<std::vector<std::string>>();
            m_globalBindings.insert(globals.begin(), globals.end());
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

const std::string& Parser::output_source_directory() const {
    return m_outputSourceDirectory;
}

const std::string& Parser::output_include_directory() const {
    return m_outputIncludeDirectory;
}

const std::string& Parser::output_painter_name() const {
    return m_outputPainterName;
}

const Parser::ShaderPaths& Parser::input_spv_paths() const {
    return m_inputSpvPaths;
}

bool Parser::print_debug_info() const {
    return m_printDebugInfo;
}

bool Parser::is_global_binding(const std::string& typeName) const {
    return m_globalBindings.find(typeName) != m_globalBindings.end();
}

}