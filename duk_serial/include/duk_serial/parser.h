//
// Created by rov on 9/30/2025.
//

#ifndef DUK_SERIAL_JSON_PARSER_H
#define DUK_SERIAL_JSON_PARSER_H

#include <duk_serial/rapidjson_import.h>

#include <string>
#include <unordered_map>
#include <memory>

namespace duk::serial {

template<typename T>
class Parser {
public:
    virtual ~Parser() = default;

    virtual void write(rapidjson::Document& document, rapidjson::Value& json, const T& value) const = 0;

    virtual void read(const rapidjson::Value& json, T& self) const = 0;
};

template<typename T>
class PrimitiveParser final : public Parser<T> {
public:
    void write(rapidjson::Document& document, rapidjson::Value& json, const T& value) const override;

    void read(const rapidjson::Value& json, T& self) const override;
};

template<typename Class>
class ClassParser final : public Parser<Class> {

    template<typename T>
    class Accessor {
    public:
        virtual ~Accessor() = default;

        virtual T& get(Class& self) const = 0;

        virtual const T& get(const Class& self) const = 0;

    };

    template<typename T>
    class MemberAccessor : public Accessor<T> {
    public:
        explicit MemberAccessor(T Class::*member);

        T& get(Class& self) const override;

        const T& get(const Class& self) const override;

    private:
        T Class::*m_member;
    };

    class Entry {
    public:
        virtual ~Entry() = default;

        virtual void write(rapidjson::Document& document, rapidjson::Value& json, const Class& self) const = 0;

        virtual void read(const rapidjson::Value& json, Class& self) const = 0;

    };

    template<typename T>
    class EntryT final : public Entry {
    public:
        explicit EntryT(std::unique_ptr<Accessor<T>> accessor);

        void write(rapidjson::Document& document, rapidjson::Value& json, const Class& self) const override;

        void read(const rapidjson::Value& json, Class& self) const override;

    private:
        std::unique_ptr<Accessor<T>> m_accessor;
    };

public:

    template<typename T>
    void add(const std::string& name, T Class::*member);

    void write(rapidjson::Document& document, rapidjson::Value& json, const Class& self) const override;

    void read(const rapidjson::Value& json, Class& self) const override;

private:
    std::unordered_map<std::string, std::unique_ptr<Entry>> m_entries;
};

// basic make_parser() specializations

struct PrimitiveParserTag {};
struct ClassParserTag {};

template<typename T>
inline constexpr bool is_json_primitive_v = std::is_same_v<T, std::string> || std::is_arithmetic_v<T>;

template<typename T>
struct select_parser_tag {
    using type = std::conditional_t<is_json_primitive_v<T>, PrimitiveParserTag, ClassParserTag>;
};

template<typename T>
using select_parser_tag_t = typename select_parser_tag<T>::type;

template<typename T>
std::unique_ptr<Parser<T>> make_parser(PrimitiveParserTag) {
    return std::make_unique<PrimitiveParser<T>>();
}

template<typename T>
void define_class_parser(ClassParser<T>& parser) = delete;

template<typename T>
std::unique_ptr<Parser<T>> make_parser(ClassParserTag) {
    auto parser = std::make_unique<ClassParser<T>>();
    define_class_parser(*parser);
    return parser;
}

template<typename T>
const Parser<T>* get_parser() {
    static const auto parser = make_parser<T>(select_parser_tag_t<T>{});
    return parser.get();
}

// PrimitiveParser //

template<typename T>
void PrimitiveParser<T>::write(rapidjson::Document& document, rapidjson::Value& json, const T& value) const {
    json.Set<T>(value, document.GetAllocator());
}

template<typename T>
void PrimitiveParser<T>::read(const rapidjson::Value& json, T& self) const {
    self = json.Get<T>();
}

// ClassParser //

template<typename Class>
template<typename T>
ClassParser<Class>::MemberAccessor<T>::MemberAccessor(T Class::* member)
    : m_member(member) {
}

template<typename Class>
template<typename T>
T& ClassParser<Class>::MemberAccessor<T>::get(Class& self) const {
    return self.*m_member;
}

template<typename Class>
template<typename T>
const T& ClassParser<Class>::MemberAccessor<T>::get(const Class& self) const {
    return self.*m_member;
}

template<typename Class>
template<typename T>
ClassParser<Class>::EntryT<T>::EntryT(std::unique_ptr<Accessor<T>> accessor)
    : m_accessor(std::move(accessor)) {
}

template<typename Class>
template<typename T>
void ClassParser<Class>::EntryT<T>::write(rapidjson::Document& document, rapidjson::Value& json, const Class& self) const {
    get_parser<T>()->write(document, json, m_accessor->get(self));
}

template<typename Class>
template<typename T>
void ClassParser<Class>::EntryT<T>::read(const rapidjson::Value& json, Class& self) const {
    get_parser<T>()->read(json, m_accessor->get(self));
}

template<typename Class>
template<typename T>
void ClassParser<Class>::add(const std::string& name, T Class::* member)  {
    m_entries.emplace(
        name,
        std::make_unique<EntryT<T>>(std::make_unique<MemberAccessor<T>>(member))
    );
}

template<typename Class>
void ClassParser<Class>::write(rapidjson::Document& document, rapidjson::Value& json, const Class& self) const {
    json.SetObject();
    for (auto& [name, entry] : m_entries) {
        rapidjson::Value entryJson;
        entry->write(document, entryJson, self);
        json.AddMember(rapidjson::StringRef(name), std::move(entryJson), document.GetAllocator());
    }
}

template<typename Class>
void ClassParser<Class>::read(const rapidjson::Value& json, Class& self) const {
    for (auto& [name, entry] : m_entries) {
        auto member = json.FindMember(name.c_str());
        if (member == json.MemberEnd()) {
            continue;
        }
        entry->read(member->value, self);
    }
}

}

#endif //DUK_SERIAL_JSON_PARSER_H