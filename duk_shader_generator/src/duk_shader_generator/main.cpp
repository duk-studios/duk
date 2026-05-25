#include <duk_shader_generator/generator.h>
#include <duk_shader_generator/options.h>

#include <cxxopts.hpp>

#include <iostream>
#include <stdexcept>

namespace {

duk::shader_generator::Options parse_options(int argc, char* argv[]) {
    cxxopts::Options cli("duk_shader_generator", "Shader data source generator for duk_rhi");

    // clang-format off
    cli.add_options()
            ("v,vert", "Vertex shader GLSL source path",                       cxxopts::value<std::string>())
            ("f,frag", "Fragment shader GLSL source path",                     cxxopts::value<std::string>())
            ("g,geom", "Geometry shader GLSL source path",                     cxxopts::value<std::string>())
            ("e,tese", "Tessellation Evaluation shader GLSL source path",      cxxopts::value<std::string>())
            ("t,tesc", "Tessellation Control shader GLSL source path",         cxxopts::value<std::string>())
            ("c,comp", "Compute shader GLSL source path",                      cxxopts::value<std::string>())
            ("n,name", "Shader name (used to derive the generated class name)", cxxopts::value<std::string>())
            ("s,src",  "Source output directory",                              cxxopts::value<std::string>())
            ("i,inc",  "Include output directory",                             cxxopts::value<std::string>())
            ("N,namespace", "Namespace for the generated class (e.g. duk::renderer)", cxxopts::value<std::string>()->default_value(""))
            ("I,include",   "GLSL include directory (repeatable)",             cxxopts::value<std::vector<std::string>>()->default_value(""))
            ("O,opt",       "Optimization level: none, performance, size",     cxxopts::value<std::string>()->default_value("performance"))
            ("d,debug", "Print debug information");
    // clang-format on

    cxxopts::ParseResult result;
    try {
        result = cli.parse(argc, argv);

        duk::shader_generator::Options options;
        options.outputSourceDirectory = result["src"].as<std::string>();
        options.outputIncludeDirectory = result["inc"].as<std::string>();
        options.shaderName = result["name"].as<std::string>();
        options.outputNamespace = result["namespace"].as<std::string>();
        options.printDebugInfo = result.count("debug") > 0;

        const auto optLevel = result["opt"].as<std::string>();
        if (optLevel == "none")              options.optimizationLevel = duk::rhi::OptimizationLevel::NONE;
        else if (optLevel == "size")         options.optimizationLevel = duk::rhi::OptimizationLevel::SIZE;
        else if (optLevel == "performance")  options.optimizationLevel = duk::rhi::OptimizationLevel::PERFORMANCE;
        else throw std::invalid_argument("unknown optimization level: '" + optLevel + "' (valid: none, performance, size)");

        if (result.count("include")) {
            options.includeDirectories = result["include"].as<std::vector<std::string>>();
            std::erase_if(options.includeDirectories, [](const std::string& s) { return s.empty(); });
        }

        if (result.count("vert")) options.inputGlslPaths[duk::rhi::ShaderModule::VERTEX]                  = result["vert"].as<std::string>();
        if (result.count("frag")) options.inputGlslPaths[duk::rhi::ShaderModule::FRAGMENT]                = result["frag"].as<std::string>();
        if (result.count("geom")) options.inputGlslPaths[duk::rhi::ShaderModule::GEOMETRY]                = result["geom"].as<std::string>();
        if (result.count("tesc")) options.inputGlslPaths[duk::rhi::ShaderModule::TESSELLATION_CONTROL]    = result["tesc"].as<std::string>();
        if (result.count("tese")) options.inputGlslPaths[duk::rhi::ShaderModule::TESSELLATION_EVALUATION] = result["tese"].as<std::string>();
        if (result.count("comp")) options.inputGlslPaths[duk::rhi::ShaderModule::COMPUTE]                 = result["comp"].as<std::string>();

        if (options.inputGlslPaths.empty()) {
            throw std::invalid_argument("no GLSL source files provided");
        }

        return options;
    } catch (const std::exception&) {
        std::cout << cli.help() << std::endl;
        throw;
    }
}

}// anonymous namespace

int main(int argc, char* argv[]) {
    try {
        const auto options = parse_options(argc, argv);
        duk::shader_generator::generate(options);
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}