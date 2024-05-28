//
// Created by Ricardo on 05/05/2024.
//

#include <duk_rhi/pipeline/shader_reflection.h>

#define SPIRV_REFLECT_USE_SYSTEM_SPIRV_H
#include <spirv_reflect.h>

namespace duk::rhi {

namespace detail {
static void check_result(SpvReflectResult result) {
    if (result != SPV_REFLECT_RESULT_SUCCESS) {
        throw std::runtime_error("error when reflecting SPIR-V: " + std::to_string(result));
    }
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

static void add_buffer_binding(SpvReflectDescriptorBinding* spvBinding, std::vector<DescriptorDescription>& bindings, ShaderModule::Bits shaderModuleBit) {
    if (bindings.size() <= spvBinding->binding) {
        bindings.resize(spvBinding->binding + 1);
    }

    auto& bindingReflection = bindings[spvBinding->binding];

    // if already added, just set this module flag and return
    if (bindingReflection.moduleMask) {
        bindingReflection.moduleMask |= shaderModuleBit;
        return;
    }

    bindingReflection.type = convert_descriptor_type(spvBinding->descriptor_type);
    bindingReflection.moduleMask = shaderModuleBit;
    bindingReflection.name = spvBinding->name;

    for (auto memberIndex = 0; memberIndex < spvBinding->block.member_count; memberIndex++) {
        const auto& spvMember = spvBinding->block.members[memberIndex];
        if (spvMember.type_description->op == SpvOpTypeRuntimeArray) {
            bindingReflection.stride = spvMember.type_description->traits.array.stride;
            for (auto instanceMemberIndex = 0; instanceMemberIndex < spvMember.member_count; instanceMemberIndex++) {
                const auto& spvInstanceMember = spvMember.members[instanceMemberIndex];
                DescriptorMemberDescription member = {};
                member.name = spvInstanceMember.name;
                member.offset = spvInstanceMember.offset;
                member.size = spvInstanceMember.size;
                member.padding = spvInstanceMember.padded_size - spvInstanceMember.size;

                bindingReflection.members.push_back(member);
            }
            return;
        }
        DescriptorMemberDescription member = {};
        member.name = spvMember.name;
        member.offset = spvMember.offset;
        member.size = spvMember.size;
        member.padding = spvMember.padded_size - spvMember.size;

        bindingReflection.members.push_back(member);
    }
    bindingReflection.stride = spvBinding->block.padded_size;
}

static void add_image_binding(SpvReflectDescriptorBinding* spvBinding, std::vector<DescriptorDescription>& bindings, ShaderModule::Bits shaderModuleBit) {
    if (bindings.size() <= spvBinding->binding) {
        bindings.resize(spvBinding->binding + 1);
    }

    auto& bindingReflection = bindings[spvBinding->binding];

    // if already added, just set this module flag and return
    if (bindingReflection.moduleMask) {
        bindingReflection.moduleMask |= shaderModuleBit;
        return;
    }

    bindingReflection.type = convert_descriptor_type(spvBinding->descriptor_type);
    bindingReflection.moduleMask = shaderModuleBit;
    bindingReflection.name = spvBinding->name;
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

void reflect_descriptor_sets(const uint8_t* data, size_t size, ShaderModule::Bits shaderModuleBit, std::vector<DescriptorSetDescription>& sets) {
    SpvReflectShaderModule module = {};
    detail::check_result(spvReflectCreateShaderModule(size, data, &module));

    uint32_t descriptorSetCount;
    detail::check_result(spvReflectEnumerateDescriptorSets(&module, &descriptorSetCount, nullptr));

    std::vector<SpvReflectDescriptorSet*> descriptorSets(descriptorSetCount);
    detail::check_result(spvReflectEnumerateDescriptorSets(&module, &descriptorSetCount, descriptorSets.data()));

    if (descriptorSetCount > sets.size()) {
        sets.resize(descriptorSetCount);
    }

    for (auto descriptorSet: descriptorSets) {
        auto& set = sets[descriptorSet->set];

        std::vector<SpvReflectDescriptorBinding*> spvBindings(descriptorSet->bindings, descriptorSet->bindings + descriptorSet->binding_count);
        for (auto spvBinding: spvBindings) {
            switch (spvBinding->descriptor_type) {
                case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                    detail::add_buffer_binding(spvBinding, set.bindings, shaderModuleBit);
                    break;
                default:
                    detail::add_image_binding(spvBinding, set.bindings, shaderModuleBit);
            }
        }
    }
}

void reflect_vertex_layout(const uint8_t* data, size_t size, VertexLayout& vertexLayout) {
    SpvReflectShaderModule module = {};
    detail::check_result(spvReflectCreateShaderModule(size, data, &module));

    uint32_t inputVariableCount;
    detail::check_result(spvReflectEnumerateEntryPointInputVariables(&module, "main", &inputVariableCount, nullptr));

    std::vector<SpvReflectInterfaceVariable*> inputVariables(inputVariableCount);
    detail::check_result(spvReflectEnumerateEntryPointInputVariables(&module, "main", &inputVariableCount, inputVariables.data()));

    detail::remove_unsupported_attributes(inputVariables);

    std::vector<VertexInput::Format> attributes(inputVariables.size());

    for (int i = 0; i < inputVariables.size(); i++) {
        auto inputVariable = inputVariables[i];
        attributes.at(inputVariable->location) = (detail::vertex_attribute_format(inputVariable->format));
    }
    vertexLayout.clear();
    vertexLayout.insert(attributes);
}

}// namespace duk::rhi
