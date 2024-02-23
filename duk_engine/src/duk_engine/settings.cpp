/// 22/02/2024
/// project.cpp

#include <duk_engine/settings.h>
#include <duk_tools/file.h>

namespace duk::engine {

Settings load_settings(const std::string& path) {
    auto content = duk::tools::File::load_text(path.c_str());

    rapidjson::Document document;

    auto& result = document.Parse(content.data());

    auto root = result.GetObject();

    Settings projectSettings = {};

    duk::json::from_json(root, projectSettings);

    return projectSettings;
}

}// namespace duk::engine