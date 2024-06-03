/// 12/09/2023
/// material.cpp

#include <duk_renderer/material/material.h>
#include <duk_renderer/renderer.h>
#include <duk_renderer/shader/shader_pipeline_pool.h>
#include <duk_renderer/image/image_pool.h>

namespace duk::renderer {

namespace detail {

static uint32_t find_descriptor(const duk::rhi::DescriptorSetDescription& descriptorSetDescription, duk::rhi::DescriptorType type, const std::string_view& name) {
    for (uint32_t binding = 0; binding < descriptorSetDescription.bindings.size(); ++binding) {
        const auto& descriptor = descriptorSetDescription.bindings[binding];
        if (descriptor.type != type && type != duk::rhi::DescriptorType::UNDEFINED) {
            continue;
        }
        if (descriptor.name == name) {
            return binding;
        }
    }
    return kInvalidMaterialLocation;
}

static uint32_t find_member(const duk::rhi::DescriptorDescription& descriptorDescription, const std::string_view& name) {
    for (uint32_t member = 0; member < descriptorDescription.members.size(); ++member) {
        if (descriptorDescription.members[member].name == name) {
            return member;
        }
    }
    return kInvalidMaterialLocation;
}

static bool extract_binding_index(const Material* material, const MaterialLocationId& binding, uint32_t& bindingIndex) {
    if (binding.valid()) {
        bindingIndex = binding.index();
        return true;
    }
    bindingIndex = material->find_binding(binding.name());
    return bindingIndex != kInvalidMaterialLocation;
}

static bool extract_member_index(const Material* material, uint32_t bindingIndex, const MaterialLocationId& member, uint32_t& memberIndex) {
    if (member.valid()) {
        memberIndex = member.index();
        return true;
    }
    memberIndex = material->find_member(bindingIndex, member.name());
    return memberIndex != kInvalidMaterialLocation;
}

static void set_member(Material* material, uint32_t bindingIndex, uint32_t memberIndex, const BufferBinding::Member& member) {
    switch (member.type) {
        case BufferBinding::Member::Type::INT:
            material->set(bindingIndex, memberIndex, member.data.intValue);
            break;
        case BufferBinding::Member::Type::UINT:
            material->set(bindingIndex, memberIndex, member.data.uintValue);
            break;
        case BufferBinding::Member::Type::FLOAT:
            material->set(bindingIndex, memberIndex, member.data.floatValue);
            break;
        case BufferBinding::Member::Type::BOOL:
            material->set(bindingIndex, memberIndex, member.data.boolValue);
            break;
        case BufferBinding::Member::Type::VEC2:
            material->set(bindingIndex, memberIndex, member.data.vec2Value);
            break;
        case BufferBinding::Member::Type::VEC3:
            material->set(bindingIndex, memberIndex, member.data.vec3Value);
            break;
        case BufferBinding::Member::Type::VEC4:
            material->set(bindingIndex, memberIndex, member.data.vec4Value);
            break;
        default:
            throw std::runtime_error("Unsupported material buffer member type");
    }
}

static duk::rhi::DescriptorType get_descriptor_type(BindingType bindingType) {
    switch (bindingType) {
        case BindingType::UNIFORM:
            return duk::rhi::DescriptorType::UNIFORM_BUFFER;
        case BindingType::INSTANCE:
            return duk::rhi::DescriptorType::STORAGE_BUFFER;
        case BindingType::IMAGE_SAMPLER:
            return duk::rhi::DescriptorType::IMAGE_SAMPLER;
        case BindingType::IMAGE:
            return duk::rhi::DescriptorType::IMAGE;
        default:
            return duk::rhi::DescriptorType::UNDEFINED;
    }
}

static void set_buffer_members(Material* material, const Binding& binding, uint32_t bindingIndex, const duk::rhi::DescriptorDescription& descriptorDescription) {
    if (!binding.data) {
        return;
    }
    const auto buffer = dynamic_cast<BufferBinding*>(binding.data.get());
    if (!buffer) {
        duk::log::fatal("Binding '{}' is not a BufferBinding", binding.name);
        return;
    }
    for (auto& member: buffer->members) {
        auto memberIndex = detail::find_member(descriptorDescription, member.name);
        if (memberIndex == kInvalidMaterialLocation) {
            duk::log::fatal("Binding '{}' member '{}' not found in Material descriptor set", binding.name, member.name);
            continue;
        }
        set_member(material, bindingIndex, memberIndex, member);
    }
}

static void add_transform_binding(MaterialData& materialData) {
    auto& transforms = materialData.bindings.emplace_back();
    transforms.name = "uTransform";
    transforms.type = BindingType::INSTANCE;
}

}// namespace detail

MaterialLocationId::MaterialLocationId()
    : m_index(kInvalidMaterialLocation) {
}

MaterialLocationId::MaterialLocationId(uint32_t index)
    : m_index(index) {
}

MaterialLocationId::MaterialLocationId(std::string name)
    : m_index(kInvalidMaterialLocation)
    , m_name(std::move(name)) {
}

MaterialLocationId::MaterialLocationId(const char* name)
    : m_index(kInvalidMaterialLocation)
    , m_name(name) {
}

uint32_t MaterialLocationId::index() const {
    return m_index;
}

std::string_view MaterialLocationId::name() const {
    return m_name;
}

bool MaterialLocationId::valid() const {
    return m_index != kInvalidMaterialLocation;
}

Material::Material(const MaterialCreateInfo& materialCreateInfo)
    : m_renderer(materialCreateInfo.renderer)
    , m_shader(materialCreateInfo.materialData.shader)
    , m_bindings(std::move(materialCreateInfo.materialData.bindings))
    , m_dirty(true) {
}

Material::~Material() = default;

ShaderPipelineResource Material::shader() const {
    return m_shader;
}

uint32_t Material::binding_count() const {
    return m_descriptorSet->size();
}

uint32_t Material::find_binding(const std::string_view& name) const {
    const auto& descriptorSetDescription = m_shader->descriptor_set_description();
    return detail::find_descriptor(descriptorSetDescription, duk::rhi::DescriptorType::UNDEFINED, name);
}

uint32_t Material::find_member(uint32_t binding, const std::string_view& name) const {
    const auto& descriptorSetDescription = m_shader->descriptor_set_description();
    return detail::find_member(descriptorSetDescription.bindings[binding], name);
}

void Material::push(const duk::objects::Id& id) {
    for (auto& [bindingIndex, instanceBuffer]: m_instanceBuffers) {
        if (instanceBuffer->expand(id.index())) {
            m_dirty = true;
            m_descriptorSet->set(bindingIndex, instanceBuffer->descriptor());
        }
        instanceBuffer->push(id);
    }
}

void Material::clear() {
    for (auto& instanceBuffer: m_instanceBuffers | std::views::values) {
        instanceBuffer->clear();
    }
}

duk::rhi::Descriptor Material::get(const MaterialLocationId& binding) const {
    if (!m_descriptorSet) {
        return {};
    }
    uint32_t bindingIndex;
    if (!detail::extract_binding_index(this, binding, bindingIndex)) {
        duk::log::fatal("Binding '{}' not found in Material descriptor get", binding.name());
        return {};
    }
    return m_descriptorSet->at(bindingIndex);
}

void Material::set(const MaterialLocationId& binding, const ImageResource& image) {
    set(binding, image->descriptor());
}

void Material::set(const MaterialLocationId& binding, const ImageResource& image, const duk::rhi::Sampler& sampler) {
    Texture texture = {image, sampler};
    set(binding, texture.descriptor());
}

void Material::set(const MaterialLocationId& binding, const void* data, uint32_t size) {
    if (!m_descriptorSet) {
        init();
    }
    uint32_t bindingIndex;
    if (!detail::extract_binding_index(this, binding, bindingIndex)) {
        duk::log::fatal("Binding '{}' not found in Material descriptor set", binding.name());
        return;
    }

    if (m_uniformBuffers.contains(bindingIndex)) {
        m_uniformBuffers.at(bindingIndex)->write(data, size);
    } else if (m_instanceBuffers.contains(bindingIndex)) {
        m_instanceBuffers.at(bindingIndex)->write(data, size);
    }
}

void Material::set(const MaterialLocationId& binding, const duk::rhi::Descriptor& descriptor) {
    if (!m_descriptorSet) {
        init();
    }
    uint32_t bindingIndex;
    if (!detail::extract_binding_index(this, binding, bindingIndex)) {
        duk::log::fatal("Binding '{}' not found in Material descriptor set", binding.name());
        return;
    }

    if (m_descriptorSet->at(bindingIndex) == descriptor) {
        return;
    }
    m_descriptorSet->set(bindingIndex, descriptor);
    m_dirty = true;
}

void Material::set(const MaterialLocationId& binding, const MaterialLocationId& member, const void* data, uint32_t size) {
    if (!m_descriptorSet) {
        init();
    }
    uint32_t bindingIndex;
    if (!detail::extract_binding_index(this, binding, bindingIndex)) {
        duk::log::fatal("Binding '{}' not found in Material descriptor set", binding.name());
        return;
    }

    uint32_t memberIndex;
    if (!detail::extract_member_index(this, bindingIndex, member, memberIndex)) {
        duk::log::fatal("Binding '{}' member '{}' not found in Material descriptor set", binding.name(), member.name());
        return;
    }

    if (m_uniformBuffers.contains(bindingIndex)) {
        m_uniformBuffers.at(bindingIndex)->write(memberIndex, data, size);
    } else if (m_instanceBuffers.contains(bindingIndex)) {
        m_instanceBuffers.at(bindingIndex)->write(memberIndex, data, size);
    }
}

void Material::set(const duk::objects::Id& id, const MaterialLocationId& binding, const void* data, uint32_t size) {
    if (!m_descriptorSet) {
        init();
    }
    uint32_t bindingIndex;
    if (!detail::extract_binding_index(this, binding, bindingIndex)) {
        duk::log::fatal("Binding '{}' not found in Material descriptor set", binding.name());
        return;
    }
    auto& instanceBuffer = m_instanceBuffers.at(bindingIndex);
    const auto index = id.index();
    if (instanceBuffer->expand(index)) {
        m_dirty = true;
        m_descriptorSet->set(bindingIndex, instanceBuffer->descriptor());
    }
    instanceBuffer->write(id, data, size);
}

void Material::set(const duk::objects::Id& id, const MaterialLocationId& binding, const MaterialLocationId& member, const void* data, uint32_t size) {
    if (!m_descriptorSet) {
        init();
    }
    uint32_t bindingIndex;
    if (!detail::extract_binding_index(this, binding, bindingIndex)) {
        duk::log::fatal("Binding '{}' not found in Material descriptor set", binding.name());
        return;
    }

    uint32_t memberIndex;
    if (!detail::extract_member_index(this, bindingIndex, member, memberIndex)) {
        duk::log::fatal("Binding '{}' member '{}' not found in Material descriptor set", binding.name(), member.name());
        return;
    }
    auto& instanceBuffer = m_instanceBuffers.at(bindingIndex);
    const auto index = id.index();
    if (instanceBuffer->expand(index)) {
        m_dirty = true;
        m_descriptorSet->set(bindingIndex, instanceBuffer->descriptor());
    }
    instanceBuffer->write(id, memberIndex, data, size);
}

void Material::update(PipelineCache& pipelineCache, duk::rhi::RenderPass* renderPass, const glm::vec2& viewport) {
    if (!m_descriptorSet) {
        init();
    }
    for (auto& uniformBuffer: m_uniformBuffers | std::views::values) {
        uniformBuffer->flush();
    }
    for (auto& [binding, instanceBuffer]: m_instanceBuffers) {
        instanceBuffer->flush();
    }
    m_shader->update(pipelineCache, renderPass, viewport);
    if (m_dirty) {
        m_descriptorSet->flush();
        m_dirty = false;
    }
}

void Material::bind(duk::rhi::CommandBuffer* commandBuffer) {
    m_shader->bind(commandBuffer);
    commandBuffer->bind_descriptor_set(m_descriptorSet.get(), 0);
}

void Material::init() {
    auto rhi = m_renderer->rhi();
    auto commandQueue = m_renderer->main_command_queue();
    // create descriptor set
    const auto& descriptorSetDescription = m_shader->descriptor_set_description();

    {
        duk::rhi::RHI::DescriptorSetCreateInfo descriptorSetCreateInfo = {};
        descriptorSetCreateInfo.description = descriptorSetDescription;
        m_descriptorSet = rhi->create_descriptor_set(descriptorSetCreateInfo);
    }

    // initialize descriptors
    for (auto& binding: m_bindings) {
        auto descriptorType = detail::get_descriptor_type(binding.type);
        if (descriptorType == duk::rhi::DescriptorType::UNDEFINED) {
            duk::log::fatal("Binding '{}' with an undefined descriptor type '{}'", binding.name);
            continue;
        }
        auto bindingIndex = detail::find_descriptor(descriptorSetDescription, descriptorType, binding.name);
        if (bindingIndex == kInvalidMaterialLocation) {
            duk::log::fatal("Binding '{}' not found in Material descriptor set", binding.name);
            continue;
        }
        auto& descriptorDescription = descriptorSetDescription.bindings[bindingIndex];
        switch (binding.type) {
            case BindingType::UNIFORM: {
                MaterialBufferCreateInfo materialBufferCreateInfo = {};
                materialBufferCreateInfo.rhi = rhi;
                materialBufferCreateInfo.commandQueue = commandQueue;
                materialBufferCreateInfo.description = descriptorDescription;
                m_uniformBuffers[bindingIndex] = std::make_unique<MaterialUniformBuffer>(materialBufferCreateInfo);
                detail::set_buffer_members(this, binding, bindingIndex, descriptorDescription);
                m_descriptorSet->set(bindingIndex, m_uniformBuffers[bindingIndex]->descriptor());
                break;
            }
            case BindingType::INSTANCE: {
                MaterialInstanceBufferCreateInfo instanceBufferCreateInfo = {};
                instanceBufferCreateInfo.rhi = rhi;
                instanceBufferCreateInfo.commandQueue = commandQueue;
                instanceBufferCreateInfo.description = descriptorSetDescription.bindings[bindingIndex];
                instanceBufferCreateInfo.instanceCount = 1;
                m_instanceBuffers[bindingIndex] = std::make_unique<MaterialInstanceBuffer>(instanceBufferCreateInfo);
                detail::set_buffer_members(this, binding, bindingIndex, descriptorDescription);
                m_descriptorSet->set(bindingIndex, m_instanceBuffers[bindingIndex]->descriptor());
                break;
            }
            case BindingType::IMAGE_SAMPLER: {
                auto textureBinding = dynamic_cast<ImageSamplerBinding*>(binding.data.get());
                m_descriptorSet->set(bindingIndex, textureBinding->descriptor());
                break;
            }
            case BindingType::IMAGE: {
                auto imageBinding = dynamic_cast<ImageBinding*>(binding.data.get());
                m_descriptorSet->set(bindingIndex, imageBinding->image->descriptor());
                break;
            }
            default:
                break;
        }
    }
    m_dirty = true;
}

std::shared_ptr<Material> create_color_material(Renderer* renderer, bool transparent) {
    auto pools = renderer->pools();
    auto shaderPool = pools->get<ShaderPipelinePool>();
    MaterialData materialData = {};
    materialData.shader = transparent ? shaderPool->transparent_color() : shaderPool->opaque_color();
    detail::add_transform_binding(materialData);
    {
        auto& properties = materialData.bindings.emplace_back();
        properties.name = "uProperties";
        properties.type = BindingType::INSTANCE;
        properties.data = []() {
            auto bindingData = std::make_unique<BufferBinding>();
            {
                auto& color = bindingData->members.emplace_back();
                color.name = "color";
                color.type = BufferBinding::Member::Type::VEC4;
                color.data.vec4Value = glm::vec4(1.0f);
            }
            return bindingData;
        }();
    }
    {
        auto& baseColor = materialData.bindings.emplace_back();
        baseColor.type = BindingType::IMAGE_SAMPLER;
        baseColor.name = "uBaseColor";
        baseColor.data = [&pools]() -> std::unique_ptr<BindingData> {
            auto imageSamplerBinding = std::make_unique<ImageSamplerBinding>();
            imageSamplerBinding->image = pools->get<ImagePool>()->white_image();
            imageSamplerBinding->sampler = kDefaultTextureSampler;
            return imageSamplerBinding;
        }();
    }

    MaterialCreateInfo materialCreateInfo = {};
    materialCreateInfo.materialData = std::move(materialData);
    materialCreateInfo.renderer = renderer;

    return std::make_shared<Material>(materialCreateInfo);
}

std::shared_ptr<Material> create_phong_material(Renderer* renderer) {
    auto pools = renderer->pools();
    MaterialData materialData = {};
    materialData.shader = pools->get<ShaderPipelinePool>()->opaque_phong();
    detail::add_transform_binding(materialData);
    {
        auto& properties = materialData.bindings.emplace_back();
        properties.name = "uProperties";
        properties.type = BindingType::INSTANCE;
        properties.data = []() {
            auto bindingData = std::make_unique<BufferBinding>();
            {
                auto& color = bindingData->members.emplace_back();
                color.name = "color";
                color.type = BufferBinding::Member::Type::VEC4;
                color.data.vec4Value = glm::vec4(1.0f);
            }
            {
                auto& shininess = bindingData->members.emplace_back();
                shininess.name = "shininess";
                shininess.type = BufferBinding::Member::Type::FLOAT;
                shininess.data.floatValue = 32.0f;
            }
            {
                auto& uvScale = bindingData->members.emplace_back();
                uvScale.name = "uvScale";
                uvScale.type = BufferBinding::Member::Type::VEC2;
                uvScale.data.vec2Value = glm::vec2(1);
            }
            return bindingData;
        }();
    }
    {
        auto& baseColor = materialData.bindings.emplace_back();
        baseColor.type = BindingType::IMAGE_SAMPLER;
        baseColor.name = "uBaseColor";
        baseColor.data = [&pools]() -> std::unique_ptr<BindingData> {
            auto imageSamplerBinding = std::make_unique<ImageSamplerBinding>();
            imageSamplerBinding->image = pools->get<ImagePool>()->white_image();
            imageSamplerBinding->sampler = kDefaultTextureSampler;
            return imageSamplerBinding;
        }();
    }
    {
        auto& specular = materialData.bindings.emplace_back();
        specular.type = BindingType::IMAGE_SAMPLER;
        specular.name = "uSpecular";
        specular.data = [&pools]() -> std::unique_ptr<BindingData> {
            auto imageSamplerBinding = std::make_unique<ImageSamplerBinding>();
            imageSamplerBinding->image = pools->get<ImagePool>()->white_image();
            imageSamplerBinding->sampler = kDefaultTextureSampler;
            return imageSamplerBinding;
        }();
    }

    MaterialCreateInfo materialCreateInfo = {};
    materialCreateInfo.materialData = std::move(materialData);
    materialCreateInfo.renderer = renderer;

    return std::make_shared<Material>(materialCreateInfo);
}

std::shared_ptr<Material> create_text_material(Renderer* renderer) {
    auto pools = renderer->pools();
    MaterialData materialData = {};
    materialData.shader = pools->get<ShaderPipelinePool>()->text();
    detail::add_transform_binding(materialData);
    {
        auto& properties = materialData.bindings.emplace_back();
        properties.name = "uProperties";
        properties.type = BindingType::UNIFORM;
        properties.data = []() {
            auto bindingData = std::make_unique<BufferBinding>();
            {
                auto& color = bindingData->members.emplace_back();
                color.name = "color";
                color.type = BufferBinding::Member::Type::VEC4;
                color.data.vec4Value = glm::vec4(1.0f);
            }
            return bindingData;
        }();
    }
    {
        auto& baseColor = materialData.bindings.emplace_back();
        baseColor.type = BindingType::IMAGE_SAMPLER;
        baseColor.name = "uBaseColor";
        baseColor.data = [&pools]() -> std::unique_ptr<BindingData> {
            auto imageSamplerBinding = std::make_unique<ImageSamplerBinding>();
            imageSamplerBinding->image = pools->get<ImagePool>()->white_image();
            imageSamplerBinding->sampler = kDefaultTextureSampler;
            return imageSamplerBinding;
        }();
    }

    MaterialCreateInfo materialCreateInfo = {};
    materialCreateInfo.materialData = std::move(materialData);
    materialCreateInfo.renderer = renderer;

    return std::make_shared<Material>(materialCreateInfo);
}

}// namespace duk::renderer