/// 14/10/2023
/// reflector.h

#ifndef DUK_PAINTER_GENERATOR_REFLECTOR_H
#define DUK_PAINTER_GENERATOR_REFLECTOR_H

#include <duk_painter_generator/parser.h>
#include <duk_rhi/pipeline/shader.h>
#include <duk_rhi/descriptor.h>

#include <cstdint>
#include <unordered_map>
#include <map>

namespace duk::painter_generator {

struct Type {
    enum Bits : uint32_t {
        UNKNOWN = 0,
        FLOAT   = 1,
        INT     = 1 << 1,
        BOOL    = 1 << 2,
        VECTOR  = 1 << 3,
        MATRIX  = 1 << 4,
        ARRAY   = 1 << 5,
        STRUCT  = 1 << 6,
    };
    static constexpr uint32_t kCount = 7;
    using Mask = uint32_t;
};

struct MemberReflection {
    std::string name;
    std::string typeName;
    size_t size;
    size_t paddedSize;
    size_t offset;
    size_t arraySize;
};

struct TypeReflection {
    std::string name;
    size_t size;
    Type::Mask typeMask;
    std::vector<MemberReflection> members;
};

struct BindingReflection {
    std::string name;
    std::string typeName;
    uint32_t binding = 0;
    uint32_t set = 0;
    duk::rhi::DescriptorType descriptorType = rhi::DescriptorType::UNDEFINED;
    duk::rhi::Shader::Module::Mask moduleMask = 0;
};

struct SetReflection {
    using Bindings = std::vector<BindingReflection>;
    uint32_t set;
    Bindings bindings;
};

class Reflector {
public:
    using TypeMap = std::unordered_map<std::string, TypeReflection>;
    using Bindings = SetReflection::Bindings;
    using Sets = std::vector<SetReflection>;
public:

    explicit Reflector(const Parser& parser);

    ~Reflector();

    const TypeMap& type_map() const;

    const Sets& sets() const;

private:

    void reflect_spv(const uint8_t* code, size_t size, duk::rhi::Shader::Module::Bits shaderModuleBit);

private:
    const Parser& m_parser;
    TypeMap m_types;
    Sets m_sets;

};

}

#endif // DUK_PAINTER_GENERATOR_REFLECTOR_H

