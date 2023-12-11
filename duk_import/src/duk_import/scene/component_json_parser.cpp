//
// Created by rov on 12/9/2023.
//
#include <duk_import/scene/component_json_parser.h>

namespace duk::import {

ComponentJsonParser::ComponentJsonParser(const ComponentParserCreateInfo& componentParserCreateInfo) :
    m_meshPool(componentParserCreateInfo.meshPool),
    m_imagePool(componentParserCreateInfo.imagePool),
    m_materialPool(componentParserCreateInfo.materialPool) {

}

void ComponentJsonParser::parse(duk::scene::Object& object, const rapidjson::Value& member) {
    auto type = member["type"].GetString();

    m_componentBuilders.at(type)(object, member);
}


}

