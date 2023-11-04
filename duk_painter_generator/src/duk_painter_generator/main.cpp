#include <duk_painter_generator/parser.h>
#include <duk_painter_generator/reflector.h>
#include <duk_painter_generator/generator.h>

#include <iostream>

static void print_reflector(duk::painter_generator::Reflector& reflector) {
    const auto& typeMap = reflector.type_map();

    std::cout << "--------------------------" << std::endl;
    std::cout << "-----------types----------" << std::endl;
    std::cout << "--------------------------" << std::endl;

    for (const auto& [typeName, type] : typeMap) {

        std::cout << "-" << typeName << std::endl;
        std::cout << "- size:        " << type.size << std::endl;
        std::cout << "- memberCount: " << type.members.size() << std::endl;
        for (const auto& member : type.members) {
            std::cout << "-- type: " << member.typeName << " name: " << member.name << std::endl;
            std::cout << "-- size: " << member.size << " paddedSize: " << member.paddedSize << " offset: " << member.offset << std::endl;
            if (member.arraySize) {
                std::cout << "-- arraySize: " << member.arraySize << std::endl;
            }
        }
    }

    std::cout << "--------------------------" << std::endl;
    std::cout << "---------bindings---------" << std::endl;
    std::cout << "--------------------------" << std::endl;

    const auto& sets = reflector.sets();
    for (const auto& set : sets) {
        std::cout << "- set buffer binding count: " << set.bindings.size() << std::endl;
        for (const auto& binding : set.bindings) {
            std::cout << "-- type: " << binding.typeName << " name: " << binding.name << std::endl;
            std::cout << "-- binding: " << binding.binding << " set: " << binding.set << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {

    try {
        duk::painter_generator::Parser parser(argc, argv);

        duk::painter_generator::Reflector reflector(parser);

        if (parser.print_debug_info()) {
            print_reflector(reflector);
        }

        duk::painter_generator::Generator generator(parser, reflector);
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what();
        return 1;
    }

    return 0;
}