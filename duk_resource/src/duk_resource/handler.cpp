/// 13/11/2023
/// handler.cpp

#include <duk_resource/handler.h>

#include <duk_log/log.h>

namespace duk::resource {

ResourceRegistry g_resourceRegistry;

ResourceRegistry* ResourceRegistry::instance() {
    return &g_resourceRegistry;
}

ResourceHandler* ResourceRegistry::find_handler(const std::string& tag) {
    auto it = m_handlers.find(tag);
    if (it == m_handlers.end()) {
        return nullptr;
    }
    return it->second.get();
}

}// namespace duk::resource