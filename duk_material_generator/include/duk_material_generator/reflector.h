/// 14/10/2023
/// reflector.h

#ifndef DUK_MATERIAL_GENERATOR_REFLECTOR_H
#define DUK_MATERIAL_GENERATOR_REFLECTOR_H

#include <duk_material_generator/parser.h>
#include <duk_rhi/descriptor.h>
#include <duk_rhi/pipeline/shader.h>
#include <duk_rhi/vertex_layout.h>

#include <cstdint>
#include <map>
#include <unordered_map>

namespace duk::material_generator {

struct Type {
    enum Bits : uint32_t {
        UNKNOWN = 0,
        FLOAT = 1,
        INT = 1 << 1,
        BOOL = 1 << 2,
        VECTOR = 1 << 3,
        MATRIX = 1 << 4,
        ARRAY = 1 << 5,
        STRUCT = 1 << 6,
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
    using Types = std::unordered_map<std::string, TypeReflection>;
    using Bindings = SetReflection::Bindings;
    using Sets = std::vector<SetReflection>;
    using Modules = std::unordered_map<duk::rhi::Shader::Module::Bits, std::vector<uint8_t>>;
    using Attributes = std::vector<duk::rhi::VertexInput::Format>;

public:
    explicit Reflector(const Parser& parser);

    ~Reflector();

    const Types& types() const;

    const Sets& sets() const;

    const Modules& modules() const;

    const Attributes& attributes() const;

private:
    void reflect_spv(const std::vector<uint8_t>& code, duk::rhi::Shader::Module::Bits shaderModuleBit);

private:
    const Parser& m_parser;
    Types m_types;
    Sets m_sets;
    Modules m_modules;
    Attributes m_attributes;
};

}// namespace duk::material_generator

#endif// DUK_MATERIAL_GENERATOR_REFLECTOR_H
