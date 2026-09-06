//
// Created by rov on 09/08/2026.
//
#include <duk_rhi/binding_layout.h>

namespace duk::rhi {

std::optional<BindingDescription> find_binding(const BindingLayout& layout, const std::string_view& name) {
    const auto it = std::ranges::find_if(layout, [name](const BindingDescription& b) { return b.name == name; });
    if (it != layout.end()) {
        return *it;
    }
    return std::nullopt;
}

std::optional<BindingMemberDescription> find_member(const std::vector<BindingMemberDescription>& members, const std::string_view& name) {
    const auto it = std::ranges::find_if(members, [name](const BindingMemberDescription& b) { return b.name == name; });
    if (it != members.end()) {
        return *it;
    }
    return std::nullopt;
}

}
