/// 13/11/2023
/// handler.cpp

#include <duk_resource/handler.h>

#include <duk_log/log.h>

#include <ranges>

namespace duk::resource {

ResourceRegistry g_resourceRegistry;

ResourceRegistry* ResourceRegistry::instance() {
    return &g_resourceRegistry;
}

Handler* ResourceRegistry::find_handler(const std::string& tag) {
    auto it = m_handlers.find(tag);
    if (it == m_handlers.end()) {
        return nullptr;
    }
    return it->second.get();
}

Handler* ResourceRegistry::find_handler_for_extension(const std::string& extension) {
    for (auto& handler: m_handlers | std::views::values) {
        if (handler->accepts(extension)) {
            return handler.get();
        }
    }
    return nullptr;
}

}// namespace duk::resource