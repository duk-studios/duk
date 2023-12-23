//
// Created by rov on 12/16/2023.
//

#include <duk_cli/commands/json_parser_generator.h>
#include <duk_tools/file.h>
#include <duk_log/log.h>
#include <string>

namespace duk::cli {

namespace detail {

const char kSerializeResourceToken[] = "DUK_SERIALIZE_RESOURCE";
const char* kTokenLimitChars = " \n;,:";

std::vector<std::string_view> extract_classes_with_token_content(const std::string& content, const char* token) {
    std::vector<std::string_view> classesContent;
    auto tokenLength = strlen(token);
    auto resourceTokenStart = content.find(token);
    while (resourceTokenStart != std::string::npos) {
        auto resourceTokenEnd = resourceTokenStart + tokenLength;

        auto classStart = content.find_first_not_of(" \n", resourceTokenEnd);
        auto classEnd = content.find("};", classStart) + 2;

        auto classStartIt = content.begin() + static_cast<std::string::difference_type>(classStart);
        auto classEndIt = content.begin() + static_cast<std::string::difference_type>(classEnd);

        classesContent.emplace_back(classStartIt, classEndIt);

        resourceTokenStart = content.find(token, classEnd);
    }
    return classesContent;
}

std::vector<std::string_view> extract_tokens_from_class(const std::string_view& classContent) {
    std::vector<std::string_view> tokens;

    auto tokenStart = classContent.find_first_not_of(" \n");
    while (tokenStart != std::string::npos) {
        auto tokenStartIt = classContent.begin() + static_cast<std::string::difference_type>(tokenStart);
        auto startChar = classContent.at(tokenStart);
        std::string::size_type tokenEnd;
        switch (startChar) {
            case ';':
            case ',':
                tokenEnd = classContent.find_first_of(detail::kTokenLimitChars, tokenStart + 1);
                break;
            case ':':
                tokenEnd = classContent.find_first_not_of(':', tokenStart);
                break;
            default:
                tokenEnd = classContent.find_first_of(detail::kTokenLimitChars, tokenStart);
                break;
        }

        if (tokenEnd == std::string::npos) {
            tokens.emplace_back(tokenStartIt, classContent.end());
            break;
        }

        auto tokenEndIt = classContent.begin() + static_cast<std::string::difference_type>(tokenEnd);

        tokens.emplace_back(tokenStartIt, tokenEndIt);

        tokenStart = classContent.find_first_not_of(" \n", tokenEnd);
    }

    return tokens;
}

}

std::string reflected_type_cpp_name(ReflectedType reflectedType) {
    switch (reflectedType) {
        case ReflectedType::INT8: return "int8_t";
        case ReflectedType::UINT8: return "uint8_t";
        case ReflectedType::INT16: return "int16_t";
        case ReflectedType::UINT16: return "uint16_t";
        case ReflectedType::INT32: return "int32_t";
        case ReflectedType::UINT32: return "uint32_t";
        case ReflectedType::INT64: return "int64_t";
        case ReflectedType::UINT64: return "uint64_t";
        case ReflectedType::FLOAT: return "float";
        case ReflectedType::DOUBLE: return "double";
        case ReflectedType::BOOL: return "bool";
        case ReflectedType::STRING: return "std::string";
        case ReflectedType::RESOURCE_ID: return "duk::pool::ResourceId";
    }
    return "";
}

Reflector::Reflector(const Parser& parser) {
    auto buffer = duk::tools::File::load_text(parser.filepath().c_str());
    std::string content(buffer.begin(), buffer.end());

    auto resourcesClasses = detail::extract_classes_with_token_content(content, detail::kSerializeResourceToken);

    for (const auto& classContent : resourcesClasses) {

        auto tokens = detail::extract_tokens_from_class(classContent);

        duk::log::debug("-----class-tokens------");
        for (const auto& token : tokens) {
            duk::log::debug("{}", token);
        }
    }
}

const std::vector<ReflectedTypeDescription>& Reflector::reflected_types() const {
    return m_reflectedTypes;
}
}