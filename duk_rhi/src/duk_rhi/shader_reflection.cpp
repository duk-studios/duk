//
// Created by Ricardo on 05/05/2024.
//

#include <duk_rhi/shader_reflection.h>

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

static void fill_buffer_members(SpvReflectDescriptorBinding* spvBinding, BufferBindingDescription& bufferDesc) {
    for (auto memberIndex = 0u; memberIndex < spvBinding->block.member_count; memberIndex++) {
        const auto& spvMember = spvBinding->block.members[memberIndex];
        if (spvMember.type_description->op == SpvOpTypeRuntimeArray) {
            bufferDesc.stride = spvMember.type_description->traits.array.stride;
            for (auto i = 0u; i < spvMember.member_count; i++) {
                const auto& inst = spvMember.members[i];
                BufferMemberDescription member;
                member.name = inst.name;
                member.offset = inst.offset;
                member.size = inst.size;
                member.padding = inst.padded_size - inst.size;
                bufferDesc.members.push_back(member);
            }
            return;
        }
        BufferMemberDescription member;
        member.name = spvMember.name;
        member.offset = spvMember.offset;
        member.size = spvMember.size;
        member.padding = spvMember.padded_size - spvMember.size;
        bufferDesc.members.push_back(member);
    }
    bufferDesc.stride = spvBinding->block.padded_size;
}

static BindingDescription make_binding_description(SpvReflectDescriptorBinding* spvBinding, ShaderModule::Bits stage) {
    BindingDescription desc;
    desc.name = spvBinding->name;
    desc.moduleMask = stage;

    switch (spvBinding->descriptor_type) {
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER: {
            BufferBindingDescription bufferDesc;
            bufferDesc.type = spvBinding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER
                ? BufferBindingType::UNIFORM_BUFFER
                : BufferBindingType::STORAGE_BUFFER;
            fill_buffer_members(spvBinding, bufferDesc);
            desc.binding = std::move(bufferDesc);
            break;
        }
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE: {
            desc.binding = ImageBindingDescription{ImageBindingType::IMAGE};
            break;
        }
        case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: {
            desc.binding = ImageBindingDescription{ImageBindingType::IMAGE_SAMPLER};
            break;
        }
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE: {
            desc.binding = ImageBindingDescription{ImageBindingType::STORAGE_IMAGE};
            break;
        }
        default:
            break;
    }

    return desc;
}

static VertexInput::Format vertex_attribute_format(SpvReflectFormat format) {
    switch (format) {
        case SPV_REFLECT_FORMAT_R16_UINT:                return VertexInput::Format::UINT16;
        case SPV_REFLECT_FORMAT_R16_SINT:                return VertexInput::Format::INT16;
        case SPV_REFLECT_FORMAT_R32_UINT:                return VertexInput::Format::UINT32;
        case SPV_REFLECT_FORMAT_R32_SINT:                return VertexInput::Format::INT32;
        case SPV_REFLECT_FORMAT_R32_SFLOAT:              return VertexInput::Format::FLOAT32;
        case SPV_REFLECT_FORMAT_R32G32_SFLOAT:           return VertexInput::Format::VEC2;
        case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:        return VertexInput::Format::VEC3;
        case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:     return VertexInput::Format::VEC4;
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

// Canonical stage iteration order for deterministic layout merging.
static constexpr ShaderModule::Bits kStageOrder[] = {
    ShaderModule::VERTEX,
    ShaderModule::TESSELLATION_CONTROL,
    ShaderModule::TESSELLATION_EVALUATION,
    ShaderModule::GEOMETRY,
    ShaderModule::FRAGMENT,
    ShaderModule::COMPUTE,
};

ShaderReflection::ShaderReflection(
    const std::unordered_map<ShaderModule::Bits, std::vector<uint8_t>>& stageSpirV) {

    for (auto stage : kStageOrder) {
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

ShaderBindingLayout ShaderReflection::binding_layout() {
    ShaderBindingLayout bindingLayout;
    std::unordered_map<std::string, uint32_t> nameToIndex;

    for (auto stage : kStageOrder) {
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
        std::sort(bindings.begin(), bindings.end(),
                  [](const SpvReflectDescriptorBinding* a, const SpvReflectDescriptorBinding* b) {
            return a->set != b->set ? a->set < b->set : a->binding < b->binding;
        });

        for (auto* spvBinding : bindings) {
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
    detail::check_result(spvReflectEnumerateEntryPointInputVariables(
        mod.get(), "main", &inputVariableCount, nullptr));

    std::vector<SpvReflectInterfaceVariable*> inputVariables(inputVariableCount);
    detail::check_result(spvReflectEnumerateEntryPointInputVariables(
        mod.get(), "main", &inputVariableCount, inputVariables.data()));

    detail::remove_unsupported_attributes(inputVariables);

    std::vector<VertexInput::Format> attributes(inputVariables.size());
    for (auto* inputVariable : inputVariables) {
        attributes.at(inputVariable->location) =
            detail::vertex_attribute_format(inputVariable->format);
    }
    vertexLayout.insert(attributes);

    return vertexLayout;
}

std::unordered_map<ShaderModule::Bits, std::vector<uint8_t>> ShaderReflection::remap_bindings(const ShaderBindingLayout& bindingLayout) {
    // Build name → logical index from the authoritative layout.
    std::unordered_map<std::string, uint32_t> nameToIndex;
    nameToIndex.reserve(bindingLayout.size());
    for (uint32_t i = 0; i < static_cast<uint32_t>(bindingLayout.size()); i++) {
        nameToIndex[bindingLayout[i].name] = i;
    }

    std::unordered_map<ShaderModule::Bits, std::vector<uint8_t>> patchedModules;

    for (auto stage : kStageOrder) {
        auto it = m_stageModules.find(stage);
        if (it == m_stageModules.end()) {
            continue;
        }

        auto& mod = it->second;

        uint32_t bindingCount = 0;
        detail::check_result(spvReflectEnumerateDescriptorBindings(mod.get(), &bindingCount, nullptr));

        std::vector<SpvReflectDescriptorBinding*> bindings(bindingCount);
        detail::check_result(spvReflectEnumerateDescriptorBindings(mod.get(), &bindingCount, bindings.data()));

        for (auto* spvBinding : bindings) {
            const uint32_t canonicalBinding = nameToIndex.at(spvBinding->name);
            constexpr uint32_t canonicalSet = 0;
            detail::check_result(spvReflectChangeDescriptorBindingNumbers(
                mod.get(), spvBinding, canonicalBinding, canonicalSet));
        }

        // Copy the patched SPIR-V words (size is unchanged — only decorations moved).
        const size_t codeSize   = spvReflectGetCodeSize(mod.get());
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

}// namespace duk::rhi
