/// 04/11/2023
/// file_generator.h

#ifndef DUK_PAINTER_GENERATOR_FILE_GENERATOR_H
#define DUK_PAINTER_GENERATOR_FILE_GENERATOR_H

#include <duk_painter_generator/reflector.h>

#include <sstream>
#include <vector>
#include <span>

namespace duk::painter_generator {

class FileGenerator {
public:

    virtual ~FileGenerator();

protected:

    static void generate_include_directives(std::ostringstream& oss, std::span<const std::string> includes);

    static void generate_namespace_start(std::ostringstream& oss, const std::string& painterName);

    static void generate_namespace_end(std::ostringstream& oss, const std::string& painterName);

    static void write_file(const std::string& content, const std::string& filepath);

};

}

#endif // DUK_PAINTER_GENERATOR_FILE_GENERATOR_H
