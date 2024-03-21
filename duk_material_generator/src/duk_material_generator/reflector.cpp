/// 14/10/2023
/// reflector.cpp

#include <duk_material_generator/reflector.h>
#include <duk_tools/file.h>

#define SPIRV_REFLECT_USE_SYSTEM_SPIRV_H
#include <spirv_reflect.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace duk::material_generator {

namespace detail {

static void check_result(SpvReflectResult result) {
    if (result != SPV_REFLECT_RESULT_SUCCESS) {
        throw std::runtime_error("error when reflecting SPIR-V: " + std::to_string(result));
    }
}

static Type::Bits convert_type_bits(SpvReflectTypeFlagBits typeBits) {
    switch (typeBits) {
        case SPV_REFLECT_TYPE_FLAG_BOOL:
            return Type::BOOL;
        case SPV_REFLECT_TYPE_FLAG_INT:
            return Type::INT;
        case SPV_REFLECT_TYPE_FLAG_FLOAT:
            return Type::FLOAT;
        case SPV_REFLECT_TYPE_FLAG_VECTOR:
            return Type::VECTOR;
        case SPV_REFLECT_TYPE_FLAG_MATRIX:
            return Type::MATRIX;
        case SPV_REFLECT_TYPE_FLAG_STRUCT:
            return Type::STRUCT;
        case SPV_REFLECT_TYPE_FLAG_ARRAY:
            return Type::ARRAY;
        default:
            return Type::UNKNOWN;
    }
}

static Type::Mask convert_type_mask(SpvReflectTypeFlags typeFlags) {
    static constexpr uint32_t kSpvReflectTypeFlagsBitsCount = 16;

    Type::Mask typeMask = 0;
    for (uint32_t i = 0; i < kSpvReflectTypeFlagsBitsCount; i++) {
        auto bit = (SpvReflectTypeFlagBits)(typeFlags & (1 << i));
        if (bit) {
            typeMask |= convert_type_bits(bit);
        }
    }
    return typeMask;
}

static duk::rhi::DescriptorType convert_descriptor_type(SpvReflectDescriptorType descriptorType) {
    switch (descriptorType) {
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
            return duk::rhi::DescriptorType::IMAGE;
        case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            return duk::rhi::DescriptorType::IMAGE_SAMPLER;
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            return duk::rhi::DescriptorType::IMAGE;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            return duk::rhi::DescriptorType::STORAGE_IMAGE;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            return duk::rhi::DescriptorType::UNIFORM_BUFFER;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            return duk::rhi::DescriptorType::STORAGE_BUFFER;
        default:
            return duk::rhi::DescriptorType::UNDEFINED;
    }
}

static std::string calculate_vector_type_name(SpvReflectTypeDescription* typeDescription) {
    const auto typeFlags = typeDescription->type_flags;
    const auto& traits = typeDescription->traits.numeric;
    std::stringstream oss;

    if (typeFlags & SPV_REFLECT_TYPE_FLAG_FLOAT) {
        if (traits.scalar.width > 32) {
            oss << 'd';
        }
    } else if (typeFlags & SPV_REFLECT_TYPE_FLAG_INT) {
        if (traits.scalar.signedness) {
            oss << 'i';
        } else {
            oss << 'u';
        }
    }
    oss << "vec" << traits.vector.component_count;
    return oss.str();
}

static std::string calculate_matrix_type_name(SpvReflectTypeDescription* typeDescription) {
    const auto& traits = typeDescription->traits.numeric;
    std::stringstream oss;

    if (traits.scalar.width > 32) {
        oss << 'd';
    }
    oss << "mat" << traits.matrix.column_count << 'x' << traits.matrix.row_count;
    return oss.str();
}

static std::string calculate_scalar_type_name(SpvReflectTypeDescription* typeDescription) {
    const auto typeFlags = typeDescription->type_flags;
    const auto& traits = typeDescription->traits.numeric;
    std::stringstream oss;
    if (typeFlags & SPV_REFLECT_TYPE_FLAG_FLOAT) {
        return traits.scalar.width > 32 ? "double" : "float";
    }
    if (typeFlags & SPV_REFLECT_TYPE_FLAG_INT) {
        return traits.scalar.signedness ? "int" : "uint";
    }
    if (typeFlags & SPV_REFLECT_TYPE_FLAG_BOOL) {
        return "bool";
    }
    return "unknown";
}

static std::string calculate_type_name(SpvReflectTypeDescription* typeDescription) {
    const auto typeFlags = typeDescription->type_flags;
    if (typeFlags & SPV_REFLECT_TYPE_FLAG_MATRIX) {
        return calculate_matrix_type_name(typeDescription);
    }
    if (typeFlags & SPV_REFLECT_TYPE_FLAG_VECTOR) {
        return calculate_vector_type_name(typeDescription);
    }
    return calculate_scalar_type_name(typeDescription);
}

static std::string type_name(SpvReflectTypeDescription* typeDescription) {
    if (typeDescription->type_name) {
        return typeDescription->type_name;
    }
    return calculate_type_name(typeDescription);
}

static void add_types_from_block(SpvReflectBlockVariable* block, Reflector::Types& types) {
    auto typeName = type_name(block->type_description);

    auto it = types.find(typeName);
    if (it != types.end()) {
        // already exists, return
        return;
    }

    TypeReflection typeReflection = {};
    typeReflection.name = typeName;
    typeReflection.size = block->size;
    typeReflection.typeMask = convert_type_mask(block->type_description->type_flags);

    std::vector<SpvReflectBlockVariable> blockMembers(block->members, block->members + block->member_count);
    for (auto& blockMember: blockMembers) {
        add_types_from_block(&blockMember, types);

        MemberReflection memberReflection = {};
        memberReflection.name = blockMember.name;
        memberReflection.typeName = type_name(blockMember.type_description);
        memberReflection.size = blockMember.size;
        memberReflection.paddedSize = blockMember.padded_size;
        memberReflection.offset = blockMember.offset;
        memberReflection.arraySize = blockMember.array.dims[0];

        typeReflection.members.push_back(memberReflection);
    }
    types.emplace(typeReflection.name, typeReflection);
}

static void add_binding(SpvReflectDescriptorBinding* spvBinding, Reflector::Bindings& bindings, duk::rhi::Shader::Module::Bits shaderModuleBit) {
    if (bindings.size() <= spvBinding->binding) {
        bindings.resize(spvBinding->binding + 1);
    }

    auto& bindingReflection = bindings[spvBinding->binding];

    // if already added, just set this module flag and return
    if (bindingReflection.moduleMask) {
        bindingReflection.moduleMask |= shaderModuleBit;
        return;
    }

    bindingReflection.name = spvBinding->name;
    bindingReflection.typeName = type_name(spvBinding->type_description);
    bindingReflection.binding = spvBinding->binding;
    bindingReflection.set = spvBinding->set;
    bindingReflection.moduleMask = shaderModuleBit;
    bindingReflection.descriptorType = convert_descriptor_type(spvBinding->descriptor_type);
}

static duk::rhi::VertexInput::Format vertex_attribute_format(SpvReflectFormat format) {
    switch (format) {
        case SPV_REFLECT_FORMAT_R16_UINT:
            return duk::rhi::VertexInput::Format::UINT16;
        case SPV_REFLECT_FORMAT_R16_SINT:
            return duk::rhi::VertexInput::Format::INT16;
        case SPV_REFLECT_FORMAT_R32_UINT:
            return duk::rhi::VertexInput::Format::UINT32;
        case SPV_REFLECT_FORMAT_R32_SINT:
            return duk::rhi::VertexInput::Format::INT32;
        case SPV_REFLECT_FORMAT_R32_SFLOAT:
            return duk::rhi::VertexInput::Format::FLOAT32;
        case SPV_REFLECT_FORMAT_R32G32_SFLOAT:
            return duk::rhi::VertexInput::Format::VEC2;
        case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:
            return duk::rhi::VertexInput::Format::VEC3;
        case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:
            return duk::rhi::VertexInput::Format::VEC4;
        default:
            throw std::runtime_error("unsupported vertex attribute format");
    }
}

static void remove_unsupported_attributes(std::vector<SpvReflectInterfaceVariable*>& attributes) {
    std::erase_if(attributes, [&attributes](SpvReflectInterfaceVariable* attribute) -> bool {
        return attribute->location > attributes.size();
    });
}

}// namespace detail

Reflector::Reflector(const Parser& parser)
    : m_parser(parser) {
    for (const auto& [module, spvPath]: m_parser.input_spv_paths()) {
        const auto spvCode = duk::tools::File::load_bytes(spvPath.c_str());

        reflect_spv(spvCode, module);
    }
}

Reflector::~Reflector() = default;

void Reflector::reflect_spv(const std::vector<uint8_t>& code, duk::rhi::Shader::Module::Bits shaderModuleBit) {
    m_modules.emplace(shaderModuleBit, code);

    SpvReflectShaderModule module = {};
    detail::check_result(spvReflectCreateShaderModule(code.size(), code.data(), &module));

    if (shaderModuleBit == rhi::Shader::Module::VERTEX) {
        uint32_t inputVariableCount;
        detail::check_result(spvReflectEnumerateEntryPointInputVariables(&module, "main", &inputVariableCount, nullptr));

        std::vector<SpvReflectInterfaceVariable*> inputVariables(inputVariableCount);
        detail::check_result(spvReflectEnumerateEntryPointInputVariables(&module, "main", &inputVariableCount, inputVariables.data()));

        detail::remove_unsupported_attributes(inputVariables);

        m_attributes.resize(inputVariables.size());

        for (int i = 0; i < inputVariables.size(); i++) {
            auto inputVariable = inputVariables[i];
            m_attributes.at(inputVariable->location) = (detail::vertex_attribute_format(inputVariable->format));
        }
    }

    uint32_t descriptorSetCount;
    detail::check_result(spvReflectEnumerateDescriptorSets(&module, &descriptorSetCount, nullptr));

    std::vector<SpvReflectDescriptorSet*> descriptorSets(descriptorSetCount);
    detail::check_result(spvReflectEnumerateDescriptorSets(&module, &descriptorSetCount, descriptorSets.data()));

    if (descriptorSetCount > m_sets.size()) {
        m_sets.resize(descriptorSetCount);
    }

    for (auto descriptorSet: descriptorSets) {
        auto& set = m_sets[descriptorSet->set];

        std::vector<SpvReflectDescriptorBinding*> spvBindings(descriptorSet->bindings, descriptorSet->bindings + descriptorSet->binding_count);
        for (auto spvBinding: spvBindings) {
            switch (spvBinding->descriptor_type) {
                case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:

                    if (spvBinding->type_description->member_count > 1) {
                        throw std::runtime_error("UBOs and SBOs can only have one member");
                    }

                    detail::add_types_from_block(&spvBinding->block, m_types);
                    detail::add_binding(spvBinding, set.bindings, shaderModuleBit);
                    break;
                default:
                    detail::add_binding(spvBinding, set.bindings, shaderModuleBit);
                    continue;
            }
        }
    }

    spvReflectDestroyShaderModule(&module);
}

const Reflector::Types& Reflector::types() const {
    return m_types;
}

const Reflector::Sets& Reflector::sets() const {
    return m_sets;
}

const Reflector::Modules& Reflector::modules() const {
    return m_modules;
}

const Reflector::Attributes& Reflector::attributes() const {
    return m_attributes;
}

}// namespace duk::material_generator