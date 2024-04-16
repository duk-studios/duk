/// 22/02/2024
/// project.cpp

#include <duk_engine/settings.h>
#include <duk_tools/file.h>

namespace duk::engine {

Settings load_settings(const std::string& path) {
    auto content = duk::tools::load_text(path);

    duk::serial::JsonReader reader(content.c_str());

    Settings settings = {};
    reader.visit(settings);

    return settings;
}

}// namespace duk::engine