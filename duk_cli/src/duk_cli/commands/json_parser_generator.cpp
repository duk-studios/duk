//
// Created by rov on 12/16/2023.
//

#include <duk_cli/commands/json_parser_generator.h>
#include <duk_tools/file.h>
#include <duk_log/log.h>
#include <rapidjson/document.h>
#include <string>
#include <regex>
#include <fstream>

namespace duk::cli {

namespace detail {

const char kSerializeResourceToken[] = "DUK_SERIALIZE_RESOURCE";
const char* kTokenLimitChars = " \n;,";

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

enum class TokenType {
    UNKNOWN,
    TYPE_KEYWORD,
    SYMBOL,
    SCOPE_START,
    SCOPE_END,
    SEMICOLON,
    COLON,
};

TokenType parse_token_type(const std::string_view& token) {
    if (token == "class" || token == "struct") {
        return TokenType::TYPE_KEYWORD;
    }
    if (token == "{") {
        return TokenType::SCOPE_START;
    }
    if (token == "}") {
        return TokenType::SCOPE_END;
    }
    if (token == ";") {
        return TokenType::SEMICOLON;
    }
    if (token == ",") {
        return TokenType::COLON;
    }
    return TokenType::SYMBOL;
}

void assert_token_type(const std::string_view& token, TokenType expectedTokenType) {
    auto actualTokenType = detail::parse_token_type(token);
    if (actualTokenType != expectedTokenType) {
        throw std::runtime_error("unexpected token: " + std::string(token));
    }
}
class Foo {
public:
    uint8_t a;
    float f;
};

template<typename T>
void from_json(const rapidjson::Value& jsonObject, T& object) = delete;

template<typename T>
T from_json(const rapidjson::Value& jsonObject) {
    T object = {};
    from_json<T>(jsonObject, object);
    return object;
}

template<>
void from_json<Foo>(const rapidjson::Value& jsonObject, Foo& object) {
    object.a = jsonObject["a"].GetUint();
    object.a = jsonObject["a"].GetInt();
    object.f = jsonObject["f"].GetFloat();
}

const char* kFromJsonTemplate = R"(
template<>
void from_json<TemplateTypeName>(const rapidjson::Value& jsonObject, TemplateTypeName& object) {
TemplateMethodDefinition
}

)";

std::string generate_from_json_member_access(const ReflectedMemberDescription& memberDescription) {
    std::ostringstream oss;
    oss << "from_json<" << memberDescription.type << ">(jsonObject[\"" << memberDescription.name << "\"])";
    return oss.str();
}

std::string generate_from_json_method_content(const ReflectedClassDescription& classDescription) {

    std::ostringstream oss;
    for (auto& member : classDescription.members) {
        oss << "    object." << member.name << " = " << generate_from_json_member_access(member) << ";" << std::endl;
    }
    return oss.str();
}

std::string generate_from_json_method(const ReflectedClassDescription& classDescription) {
    auto content = std::regex_replace(kFromJsonTemplate, std::regex("TemplateTypeName"), classDescription.name);
    content = std::regex_replace(content, std::regex("TemplateMethodDefinition"), generate_from_json_method_content(classDescription));
    return content;
}

}

Reflector::Reflector(const std::string& content) {

    auto resourcesClasses = detail::extract_classes_with_token_content(content, detail::kSerializeResourceToken);

    for (const auto& classContent : resourcesClasses) {

        auto tokens = detail::extract_tokens_from_class(classContent);

        ReflectedClassDescription reflectedClassDescription = {};

        for (auto tokenIt = tokens.begin(); tokenIt != tokens.end(); ++tokenIt) {
            auto& token = *tokenIt;

            auto tokenType = detail::parse_token_type(token);
            switch (tokenType) {
                case detail::TokenType::TYPE_KEYWORD:
                {
                    tokenIt++;
                    token = *tokenIt;
                    detail::assert_token_type(token, detail::TokenType::SYMBOL);
                    reflectedClassDescription.name = token;
                }
                    break;
                case detail::TokenType::SYMBOL:
                {
                    ReflectedMemberDescription reflectedTypeDescription = {};
                    reflectedTypeDescription.type = token;
                    tokenIt++;
                    token = *tokenIt;
                    detail::assert_token_type(token, detail::TokenType::SYMBOL);
                    reflectedTypeDescription.name = token;

                    reflectedClassDescription.members.emplace_back(std::move(reflectedTypeDescription));
                }
                    break;
                default:
                    duk::log::info("ignored token: {}", std::string(token));
                    break;
            }
        }
        m_reflectedClasses.emplace_back(std::move(reflectedClassDescription));
    }
}

const std::vector<ReflectedClassDescription>& Reflector::reflected_types() const {
    return m_reflectedClasses;
}

void generate_json_parser(const GenerateJsonParserInfo& generateJsonParserInfo) {
    auto buffer = duk::tools::File::load_text(generateJsonParserInfo.inputFilepath.c_str());
    std::string content(buffer.begin(), buffer.end());
    Reflector reflector(content);

    std::ostringstream oss;
    for (const auto& classDescription : reflector.reflected_types()) {
        oss << detail::generate_from_json_method(classDescription) << std::endl;
    }

    std::ofstream file(generateJsonParserInfo.outputFilepath);
    file << oss.str();
}

}