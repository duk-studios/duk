//
// Created by Ricardo on 05/05/2024.
//

#include <duk_rhi/shader_reflection.h>
#include <duk_rhi/binding_layout.h>
#include <duk_rhi/command_context.h>

#include <duk_log/log.h>

#define SPIRV_REFLECT_USE_SYSTEM_SPIRV_H
#include <spirv_reflect.h>

#include <algorithm>
#include <stdexcept>

namespace duk::rhi {

namespace detail {

static void check_result(SpvReflectResult result) {
    if (result != SPV_REFLECT_RESULT_SUCCESS) {
        throw std::runtime_error("error when reflecting SPIR-V: " + std::to_string(result));
    }
}

static std::string member_type_name(SpvReflectTypeDescription* typeDesc) {
    if (!typeDesc) {
        return "unknown";
    }
    if ((typeDesc->type_flags & SPV_REFLECT_TYPE_FLAG_STRUCT) && typeDesc->type_name) {
        return typeDesc->type_name;
    }
    const auto& num = typeDesc->traits.numeric;
    if (typeDesc->type_flags & SPV_REFLECT_TYPE_FLAG_MATRIX) {
        std::string prefix = (num.scalar.width > 32) ? "d" : "";
        const auto cols = num.matrix.column_count;
        const auto rows = num.matrix.row_count;
        if (cols == rows) {
            return prefix + "mat" + std::to_string(cols);
        }
        return prefix + "mat" + std::to_string(cols) + "x" + std::to_string(rows);
    }
    if (typeDesc->type_flags & SPV_REFLECT_TYPE_FLAG_VECTOR) {
        std::string prefix;
        if ((typeDesc->type_flags & SPV_REFLECT_TYPE_FLAG_FLOAT) && num.scalar.width > 32) {
            prefix = "d";
        } else if (typeDesc->type_flags & SPV_REFLECT_TYPE_FLAG_INT) {
            prefix = num.scalar.signedness ? "i" : "u";
        }
        return prefix + "vec" + std::to_string(num.vector.component_count);
    }
    if (typeDesc->type_flags & SPV_REFLECT_TYPE_FLAG_FLOAT) {
        return num.scalar.width > 32 ? "double" : "float";
    }
    if (typeDesc->type_flags & SPV_REFLECT_TYPE_FLAG_INT) {
        return num.scalar.signedness ? "int" : "uint";
    }
    if (typeDesc->type_flags & SPV_REFLECT_TYPE_FLAG_BOOL) {
        return "bool";
    }
    return "unknown";
}

static BindingMemberDescription make_buffer_member(const SpvReflectBlockVariable& spvMember) {
    BindingMemberDescription member;
    member.name = spvMember.name;
    member.offset = spvMember.offset;
    member.size = spvMember.size;
    member.padding = spvMember.padded_size - spvMember.size;
    member.typeName = member_type_name(spvMember.type_description);
    return member;
}

static void fill_buffer_members(SpvReflectDescriptorBinding* spvBinding, BindingDescription& desc) {
    for (auto memberIndex = 0u; memberIndex < spvBinding->block.member_count; memberIndex++) {
        const auto& spvMember = spvBinding->block.members[memberIndex];
        if (spvMember.type_description->op == SpvOpTypeRuntimeArray) {
            desc.stride = spvMember.type_description->traits.array.stride;
            desc.size = desc.stride;
            for (auto i = 0u; i < spvMember.member_count; i++) {
                desc.members.push_back(make_buffer_member(spvMember.members[i]));
            }
            return;
        }
        desc.members.push_back(make_buffer_member(spvMember));
    }
    desc.size = spvBinding->block.size;
    desc.stride = spvBinding->block.padded_size;
}

static BindingType convert_binding_type(SpvReflectDescriptorType type) {
    BindingType result;

    switch (type) {
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
            result = BindingType::IMAGE;
            break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            result = BindingType::SAMPLER_IMAGE;
            break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            result = BindingType::STORAGE_IMAGE;
            break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
            result = BindingType::UNIFORM_BUFFER;
            break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
            result = BindingType::STORAGE_BUFFER;
            break;
        default:
            throw std::runtime_error("Unsupported binding type: " + std::to_string(type));
    }
    return result;
}

static BindingDescription make_binding_description(SpvReflectDescriptorBinding* spvBinding, ShaderModule::Bits stage) {
    BindingDescription desc = {};
    desc.name = spvBinding->name;
    desc.moduleMask = stage;
    desc.type = convert_binding_type(spvBinding->descriptor_type);
    if (desc.type == BindingType::UNIFORM_BUFFER || desc.type == BindingType::STORAGE_BUFFER) {
        fill_buffer_members(spvBinding, desc);
    }

    return desc;
}

static VertexInput::Format vertex_attribute_format(SpvReflectFormat format) {
    switch (format) {
        case SPV_REFLECT_FORMAT_R16_UINT:
            return VertexInput::Format::UINT16;
        case SPV_REFLECT_FORMAT_R16_SINT:
            return VertexInput::Format::INT16;
        case SPV_REFLECT_FORMAT_R32_UINT:
            return VertexInput::Format::UINT32;
        case SPV_REFLECT_FORMAT_R32_SINT:
            return VertexInput::Format::INT32;
        case SPV_REFLECT_FORMAT_R32_SFLOAT:
            return VertexInput::Format::FLOAT32;
        case SPV_REFLECT_FORMAT_R32G32_SFLOAT:
            return VertexInput::Format::VEC2;
        case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:
            return VertexInput::Format::VEC3;
        case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:
            return VertexInput::Format::VEC4;
        default:
            throw std::runtime_error("unsupported vertex attribute format");
    }
}

static void remove_unsupported_attributes(std::vector<SpvReflectInterfaceVariable*>& attributes) {
    std::erase_if(attributes, [&attributes](SpvReflectInterfaceVariable* attribute) -> bool {
        return attribute->location > attributes.size();
    });
}

static void extract_struct(StructTypeMap& types, SpvReflectBlockVariable* block) {
    if (!block->type_description) {
        return;
    }

    const bool isStruct = block->type_description->type_flags & SPV_REFLECT_TYPE_FLAG_STRUCT;
    if (!isStruct || block->member_count == 0) {
        return;
    }

    const char* rawName = block->type_description->type_name;
    if (!rawName || rawName[0] == '\0') {
        return;
    }

    const std::string typeName(rawName);
    if (types.count(typeName)) {
        for (uint32_t i = 0; i < block->member_count; i++) {
            extract_struct(types, &block->members[i]);
        }
        return;
    }

    const bool isArray = block->type_description->type_flags & SPV_REFLECT_TYPE_FLAG_ARRAY;
    const uint32_t structSize = isArray ? block->type_description->traits.array.stride : block->size;

    StructDefinition def;
    def.name = typeName;
    def.size = structSize;

    for (uint32_t i = 0; i < block->member_count; i++) {
        auto& m = block->members[i];

        StructMember member;
        member.name = m.name ? m.name : "";
        member.typeName = detail::member_type_name(m.type_description);
        member.offset = m.offset;
        member.size = m.size;
        member.padding = m.padded_size > m.size ? m.padded_size - m.size : 0;
        def.members.push_back(member);

        extract_struct(types, &m);
    }

    types.emplace(typeName, std::move(def));
}

}// namespace detail

// Canonical stage iteration order for deterministic layout merging.
static constexpr ShaderModule::Bits kStageOrder[] = {
        ShaderModule::VERTEX, ShaderModule::TESSELLATION_CONTROL, ShaderModule::TESSELLATION_EVALUATION, ShaderModule::GEOMETRY, ShaderModule::FRAGMENT, ShaderModule::COMPUTE,
};

ShaderReflection::ShaderReflection(const std::unordered_map<ShaderModule::Bits, std::vector<uint8_t>>& stageSpirV) {
    for (auto stage: kStageOrder) {
        auto it = stageSpirV.find(stage);
        if (it == stageSpirV.end()) {
            continue;
        }
        const auto& code = it->second;

        auto& mod = m_stageModules[stage];
        mod = std::unique_ptr<SpvReflectShaderModule, SpvReflectShaderModuleDeleter>(new SpvReflectShaderModule());
        detail::check_result(spvReflectCreateShaderModule(code.size(), code.data(), mod.get()));
    }
}

ShaderReflection::~ShaderReflection() {
    // spv module destruction is delegated to the deleter
    m_stageModules.clear();
}

BindingLayout ShaderReflection::binding_layout() {
    BindingLayout bindingLayout;
    std::unordered_map<std::string, uint32_t> nameToIndex;

    for (auto stage: kStageOrder) {
        auto it = m_stageModules.find(stage);
        if (it == m_stageModules.end()) {
            continue;
        }
        auto& mod = it->second;

        uint32_t bindingCount = 0;
        detail::check_result(spvReflectEnumerateDescriptorBindings(mod.get(), &bindingCount, nullptr));

        std::vector<SpvReflectDescriptorBinding*> bindings(bindingCount);
        detail::check_result(spvReflectEnumerateDescriptorBindings(mod.get(), &bindingCount, bindings.data()));

        // Sort by (set, binding) for a stable per-stage slot assignment.
        std::ranges::sort(bindings, [](const SpvReflectDescriptorBinding* a, const SpvReflectDescriptorBinding* b) {
            return a->set != b->set ? a->set < b->set : a->binding < b->binding;
        });

        for (auto* spvBinding: bindings) {
            const std::string name = spvBinding->name;
            auto found = nameToIndex.find(name);
            if (found == nameToIndex.end()) {
                auto idx = static_cast<uint32_t>(bindingLayout.size());
                nameToIndex[name] = idx;
                bindingLayout.push_back(detail::make_binding_description(spvBinding, stage));
            } else {
                // Descriptor shared across stages — accumulate stage mask.
                bindingLayout[found->second].moduleMask |= stage;
            }
        }
    }

    return bindingLayout;
}

VertexLayout ShaderReflection::vertex_layout() {
    VertexLayout vertexLayout;

    auto it = m_stageModules.find(ShaderModule::VERTEX);
    if (it == m_stageModules.end()) {
        return vertexLayout;
    }
    auto& mod = it->second;

    uint32_t inputVariableCount = 0;
    detail::check_result(spvReflectEnumerateEntryPointInputVariables(mod.get(), "main", &inputVariableCount, nullptr));

    std::vector<SpvReflectInterfaceVariable*> inputVariables(inputVariableCount);
    detail::check_result(spvReflectEnumerateEntryPointInputVariables(mod.get(), "main", &inputVariableCount, inputVariables.data()));

    detail::remove_unsupported_attributes(inputVariables);

    for (auto* inputVariable: inputVariables) {
        vertexLayout.set(inputVariable->location, detail::vertex_attribute_format(inputVariable->format));
    }

    return vertexLayout;
}

std::unordered_map<ShaderModule::Bits, std::vector<uint8_t>> ShaderReflection::remap_bindings(const BindingLayout& bindingLayout) {
    // Build name → logical index from the authoritative layout.
    std::unordered_map<std::string, uint32_t> nameToIndex;
    nameToIndex.reserve(bindingLayout.size());
    for (uint32_t i = 0; i < static_cast<uint32_t>(bindingLayout.size()); i++) {
        nameToIndex[bindingLayout[i].name] = i;
    }

    std::unordered_map<ShaderModule::Bits, std::vector<uint8_t>> patchedModules;

    for (auto stage: kStageOrder) {
        auto it = m_stageModules.find(stage);
        if (it == m_stageModules.end()) {
            continue;
        }

        auto& mod = it->second;

        uint32_t bindingCount = 0;
        detail::check_result(spvReflectEnumerateDescriptorBindings(mod.get(), &bindingCount, nullptr));

        std::vector<SpvReflectDescriptorBinding*> bindings(bindingCount);
        detail::check_result(spvReflectEnumerateDescriptorBindings(mod.get(), &bindingCount, bindings.data()));

        for (auto* spvBinding: bindings) {
            const uint32_t canonicalBinding = nameToIndex.at(spvBinding->name);
            constexpr uint32_t canonicalSet = 0;
            detail::check_result(spvReflectChangeDescriptorBindingNumbers(mod.get(), spvBinding, canonicalBinding, canonicalSet));
        }

        // Copy the patched SPIR-V words (size is unchanged — only decorations moved).
        const size_t codeSize = spvReflectGetCodeSize(mod.get());
        const uint32_t* codePtr = spvReflectGetCode(mod.get());
        auto& patched = patchedModules[stage];
        patched.resize(codeSize);
        std::memcpy(patched.data(), codePtr, codeSize);
    }

    return patchedModules;
}

void ShaderReflection::SpvReflectShaderModuleDeleter::operator()(SpvReflectShaderModule* module) const noexcept {
    spvReflectDestroyShaderModule(module);
    delete module;
}

StructTypeMap ShaderReflection::extract_struct_types() {
    StructTypeMap types;

    for (auto stage: kStageOrder) {
        auto it = m_stageModules.find(stage);
        if (it == m_stageModules.end()) {
            continue;
        }
        auto& mod = it->second;

        uint32_t bindingCount = 0;
        detail::check_result(spvReflectEnumerateDescriptorBindings(mod.get(), &bindingCount, nullptr));

        std::vector<SpvReflectDescriptorBinding*> bindings(bindingCount);
        detail::check_result(spvReflectEnumerateDescriptorBindings(mod.get(), &bindingCount, bindings.data()));

        for (auto* spvBinding: bindings) {
            if (spvBinding->descriptor_type != SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER &&
                spvBinding->descriptor_type != SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
                continue;
            }

            detail::extract_struct(types, &spvBinding->block);
        }
    }

    return types;
}

}// namespace duk::rhi
