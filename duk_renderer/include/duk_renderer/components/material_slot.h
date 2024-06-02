//
// Created by Ricardo on 02/06/2024.
//

#ifndef DUK_RENDERER_MATERIAL_SLOT_H
#define DUK_RENDERER_MATERIAL_SLOT_H

#include <duk_renderer/material/material.h>

namespace duk::renderer {

struct MaterialSlot {
    MaterialResource material;
};

}// namespace duk::renderer

namespace duk::serial {

template<>
inline void from_json(const rapidjson::Value& json, duk::renderer::MaterialSlot& slot) {
    from_json_member(json, "material", slot.material);
}

template<>
inline void to_json(rapidjson::Document& document, rapidjson::Value& json, const duk::renderer::MaterialSlot& slot) {
    to_json_member(document, json, "material", slot.material);
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::renderer::MaterialSlot& slot) {
    solver->solve(slot.material);
}

}// namespace duk::resource

#endif//DUK_RENDERER_MATERIAL_SLOT_H
