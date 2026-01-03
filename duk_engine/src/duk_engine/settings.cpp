/// 22/02/2024
/// project.cpp

#include <duk_engine/settings.h>
#include <duk_tools/file.h>

#include <duk_serial/json.h>

namespace duk::engine {

Settings load_settings(const std::string& path) {
    auto content = duk::tools::load_text(path);

    Settings settings = {};
    duk::serial::json_read(content, settings);

    return settings;
}

}// namespace duk::engine