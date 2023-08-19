//
// Created by Ricardo on 19/08/2023.
//

#include <duk_rhi/rhi.h>
#include <duk_renderer/mesh.h>


int main() {

    duk::rhi::RHICreateInfo rhiCreateInfo = {};
    rhiCreateInfo.api = duk::rhi::API::VULKAN;
    rhiCreateInfo.engineName = "duk";
    rhiCreateInfo.engineVersion = 1;
    rhiCreateInfo.applicationName = "duk_renderer_test";
    rhiCreateInfo.applicationVersion = 1;
    rhiCreateInfo.deviceIndex = 0;

    auto expectedRHI = duk::rhi::RHI::create_rhi(rhiCreateInfo);

    if (!expectedRHI) {
        return 1;
    }

    auto rhi = std::move(expectedRHI.value());



    duk::renderer::MeshBufferPoolCreateInfo meshBufferPoolCreateInfo = {}

}