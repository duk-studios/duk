//
// Created by Ricardo on 19/06/2024.
//

#ifndef DUK_RENDERER_SHADER_MODULE_H
#define DUK_RENDERER_SHADER_MODULE_H

#include <duk_resource/resource.h>

namespace duk::renderer {

struct ShaderModuleCreateInfo {
    const uint8_t* code;
    size_t size;
};

class ShaderModule {
public:
    ShaderModule(const ShaderModuleCreateInfo& createInfo);

    const uint8_t* data() const;

    size_t size() const;

private:
    std::vector<uint8_t> m_code;
};

using ShaderModuleResource = duk::resource::Handle<ShaderModule>;

}// namespace duk::renderer

#endif//DUK_RENDERER_SHADER_MODULE_H
