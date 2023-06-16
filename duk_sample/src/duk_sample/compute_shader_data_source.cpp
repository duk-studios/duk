/// 16/06/2023
/// compute_shader_data_source.cpp

#include <duk_sample/compute_shader_data_source.h>
#include <duk_sample/application.h>

namespace duk::sample {

ComputeShaderDataSource::ComputeShaderDataSource() {
    insert_spir_v_code(duk::renderer::Shader::Module::COMPUTE, Application::load_bytes("compute.comp.spv"));

    duk::renderer::DescriptorSetDescription descriptorSetDescription;
    auto& transformBinding = descriptorSetDescription.bindings.emplace_back();
    transformBinding.type = renderer::DescriptorType::STORAGE_IMAGE;
    transformBinding.moduleMask = duk::renderer::Shader::Module::COMPUTE;

    insert_descriptor_set_description(descriptorSetDescription);

    update_hash();
}


}
