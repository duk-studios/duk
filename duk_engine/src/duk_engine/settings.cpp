/// 22/02/2024
/// project.cpp

#include <duk_engine/settings.h>
#include <duk_tools/file.h>

namespace duk::engine {

Settings load_settings(const std::string& path) {
    auto content = duk::tools::File::load_text(path.c_str());

    Settings settings = {};
    duk::serial::read_json(content, settings);

    return settings;
}

}// namespace duk::engine