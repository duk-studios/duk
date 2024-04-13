//
// Created by rov on 12/2/2023.
//

#ifndef DUK_MATERIAL_HANDLER_H
#define DUK_MATERIAL_HANDLER_H

#include <duk_resource/handler.h>

#include <duk_renderer/material/material_pool.h>

#include <filesystem>

namespace duk::renderer {

struct MaterialHandlerCreateInfo {
    MaterialPool* materialPool;
};

class MaterialHandler : public duk::resource::ResourceHandler {
public:
    explicit MaterialHandler(const MaterialHandlerCreateInfo& materialHandlerCreateInfo);

    const std::string& tag() const override;

    void load(const duk::resource::Id& id, const std::filesystem::path& path) override;

    void solve_dependencies(const duk::resource::Id& id, duk::resource::DependencySolver& dependencySolver) override;

    void solve_references(const duk::resource::Id& id, duk::resource::ReferenceSolver& referenceSolver) override;

private:
    MaterialPool* m_materialPool;
};

}// namespace duk::renderer

#endif//DUK_MATERIAL_HANDLER_H
