/// 14/10/2023
/// parser.cpp

#include <duk_shader_generator/parser.h>

#include <cxxopts.hpp>
#include <iostream>

namespace duk::material_generator {

Parser::Parser(int argc, char* argv[])
    : m_printDebugInfo(false) {
    using duk::rhi::Shader;

    cxxopts::Options options("duk_shader_generator", "Material generator for duk_renderer");

    // clang-format off
    options.add_options()
            ("v, vert", "Vertex shader path", cxxopts::value<std::string>())
            ("f, frag", "Fragment shader path", cxxopts::value<std::string>())
            ("g, geom", "Geometry shader path", cxxopts::value<std::string>())
            ("e, tese", "Tesselation Evaluation shader path", cxxopts::value<std::string>())
            ("t, tesc", "Tesselation Control shader path", cxxopts::value<std::string>())
            ("c, comp", "Compute shader path", cxxopts::value<std::string>())
            ("n, name", "Material name", cxxopts::value<std::string>())
            ("s, src", "Source output directory", cxxopts::value<std::string>())
            ("i, inc", "Include output directory", cxxopts::value<std::string>())
            ("d, debug", "Print debug information");
    // clang-format on

    cxxopts::ParseResult result;
    try {
        result = options.parse(argc, argv);

        m_outputSourceDirectory = result["src"].as<std::string>();
        m_outputIncludeDirectory = result["inc"].as<std::string>();
        m_outputMaterialName = result["name"].as<std::string>();

        if (result.count("vert")) {
            m_inputSpvPaths[rhi::ShaderModule::VERTEX] = result["vert"].as<std::string>();
        }
        if (result.count("frag")) {
            m_inputSpvPaths[rhi::ShaderModule::FRAGMENT] = result["frag"].as<std::string>();
        }
        if (result.count("geom")) {
            m_inputSpvPaths[rhi::ShaderModule::GEOMETRY] = result["geom"].as<std::string>();
        }
        if (result.count("tesc")) {
            m_inputSpvPaths[rhi::ShaderModule::TESSELLATION_CONTROL] = result["tesc"].as<std::string>();
        }
        if (result.count("tese")) {
            m_inputSpvPaths[rhi::ShaderModule::TESSELLATION_EVALUATION] = result["tese"].as<std::string>();
        }
        if (result.count("comp")) {
            m_inputSpvPaths[rhi::ShaderModule::COMPUTE] = result["comp"].as<std::string>();
        }
        if (result.count("debug")) {
            m_printDebugInfo = true;
        }

        if (m_inputSpvPaths.empty()) {
            throw std::invalid_argument("no SPIR-V sources provided");
        }
    } catch (const std::exception&) {
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

const std::string& Parser::output_material_name() const {
    return m_outputMaterialName;
}

const Parser::ShaderPaths& Parser::input_spv_paths() const {
    return m_inputSpvPaths;
}

bool Parser::print_debug_info() const {
    return m_printDebugInfo;
}

}// namespace duk::material_generator