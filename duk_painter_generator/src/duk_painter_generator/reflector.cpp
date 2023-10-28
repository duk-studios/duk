/// 14/10/2023
/// reflector.cpp

#include <duk_painter_generator/reflector.h>
#include <duk_tools/file.h>

#include <spirv_reflect.h>

#include <stdexcept>
#include <sstream>

namespace duk::painter_generator {

namespace detail {

static void check_result(SpvReflectResult result) {
    if (result != SPV_REFLECT_RESULT_SUCCESS) {
        throw std::runtime_error("error when reflecting SPIR-V: " + std::to_string(result));
    }
}

static Type::Bits convert_type_bits(SpvReflectTypeFlagBits typeBits) {
    switch (typeBits) {
        case SPV_REFLECT_TYPE_FLAG_BOOL: return Type::BOOL;
        case SPV_REFLECT_TYPE_FLAG_INT: return Type::INT;
        case SPV_REFLECT_TYPE_FLAG_FLOAT: return Type::FLOAT;
        case SPV_REFLECT_TYPE_FLAG_VECTOR: return Type::VECTOR;
        case SPV_REFLECT_TYPE_FLAG_MATRIX: return Type::MATRIX;
        case SPV_REFLECT_TYPE_FLAG_STRUCT: return Type::STRUCT;
        case SPV_REFLECT_TYPE_FLAG_ARRAY: return Type::ARRAY;
        default: return Type::UNKNOWN;
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

static std::string calculate_vector_type_name(SpvReflectTypeDescription* typeDescription) {
    const auto typeFlags = typeDescription->type_flags;
    const auto& traits = typeDescription->traits.numeric;
    std::stringstream oss;

    if (typeFlags & SPV_REFLECT_TYPE_FLAG_FLOAT) {
        if (traits.scalar.width > 32) {
            oss << 'd';
        }
    }
    else if (typeFlags & SPV_REFLECT_TYPE_FLAG_INT) {
        if (traits.scalar.signedness) {
            oss << 'i';
        }
        else {
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

static void add_types_from_block(SpvReflectBlockVariable* block, Reflector::TypeMap& types) {
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
    for (auto& blockMember : blockMembers) {

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
    auto it = bindings.find(spvBinding->binding);
    if (it != bindings.end()) {
        it->second.moduleMask |= shaderModuleBit;
        return;
    }

    auto& bindingReflection = bindings[spvBinding->binding];
    bindingReflection.name = spvBinding->name;
    bindingReflection.typeName = type_name(spvBinding->type_description);
    bindingReflection.binding = spvBinding->binding;
    bindingReflection.set = spvBinding->set;
    bindingReflection.moduleMask = shaderModuleBit;
}

}

Reflector::Reflector(const Parser& parser) {
    for (const auto& [module, spvPath] : parser.input_spv_paths()) {

        const auto spvCode = duk::tools::File::load_bytes(spvPath.c_str());

        reflect_spv(spvCode.data(), spvCode.size(), module);
    }
}

Reflector::~Reflector() = default;

void Reflector::reflect_spv(const uint8_t* code, size_t size, duk::rhi::Shader::Module::Bits shaderModuleBit) {

    SpvReflectShaderModule module = {};
    detail::check_result(spvReflectCreateShaderModule(size, code, &module));

    uint32_t descriptorSetCount;
    detail::check_result(spvReflectEnumerateDescriptorSets(&module, &descriptorSetCount, nullptr));

    std::vector<SpvReflectDescriptorSet*> descriptorSets(descriptorSetCount);
    detail::check_result(spvReflectEnumerateDescriptorSets(&module, &descriptorSetCount, descriptorSets.data()));

    if (descriptorSetCount > m_sets.size()) {
        m_sets.resize(descriptorSetCount);
    }

    for (auto descriptorSet : descriptorSets) {
        auto& set = m_sets[descriptorSet->set];

        std::vector<SpvReflectDescriptorBinding*> spvBindings(descriptorSet->bindings, descriptorSet->bindings + descriptorSet->binding_count);
        for (auto spvBinding : spvBindings) {
            if (spvBinding->type_description->member_count > 1) {
                throw std::runtime_error("UBOs and SBOs can only have one member");
            }
            detail::add_types_from_block(&spvBinding->block, m_types);
            switch (spvBinding->descriptor_type) {
                case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                    detail::add_binding(spvBinding, set.storageBuffers, shaderModuleBit);
                    break;
                case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                    detail::add_binding(spvBinding, set.uniformBuffers, shaderModuleBit);
                    break;
                default:
                    throw std::runtime_error("unsupported descriptor type found");
            }
        }
    }

    spvReflectDestroyShaderModule(&module);
}

const Reflector::TypeMap& Reflector::type_map() const {
    return m_types;
}

const Reflector::Sets& Reflector::sets() const {
    return m_sets;
}

}