/// 13/11/2023
/// handler.h

#ifndef DUK_RESOURCE_HANDLER_H
#define DUK_RESOURCE_HANDLER_H

#include <duk_resource/file.h>
#include <duk_resource/pool.h>
#include <duk_resource/handle.h>
#include <duk_resource/resources.h>
#include <duk_resource/solver/dependency_solver.h>
#include <duk_resource/solver/reference_solver.h>

#include <filesystem>

#include <duk_tools/file.h>

namespace duk::resource {

class Handler {
public:
    virtual ~Handler() = default;

    virtual const std::string& tag() const = 0;

    virtual bool accepts(const std::string& extension) const = 0;

    virtual Handle<void> load(duk::tools::Globals* globals, Pools& pools, const Id& id, const std::filesystem::path& path, LoadMode loadMode) = 0;

    virtual void solve_dependencies(const Handle<void>& handle, std::set<Id>& dependencies) = 0;

    virtual void solve_references(const Handle<void>& handle, Pools& pools) = 0;
};

template<typename T>
class HandlerT : public Handler {
public:
    explicit HandlerT(const char* tag);

    const std::string& tag() const final;

    Handle<void> load(duk::tools::Globals* globals, Pools& pools, const Id& id, const std::filesystem::path& path, LoadMode loadMode) override;

    void solve_dependencies(const Handle<void>& handle, std::set<Id>& dependencies) override;

    void solve_references(const Handle<void>& handle, Pools& pools) override;

protected:
    virtual std::shared_ptr<T> load_from_memory(duk::tools::Globals* globals, const void* data, size_t size) = 0;

private:
    std::string m_tag;
};

template<typename T>
class TextHandlerT : public HandlerT<T> {
public:
    using HandlerT<T>::HandlerT;

protected:
    std::shared_ptr<T> load_from_memory(duk::tools::Globals* globals, const void* data, size_t size) override;

    virtual std::shared_ptr<T> load_from_text(duk::tools::Globals* globals, const std::string_view& text) = 0;
};

class ResourceRegistry {
public:
    static ResourceRegistry* instance();

    template<typename THandler>
    void add();

    Handler* find_handler(const std::string& tag);

    Handler* find_handler_for_extension(const std::string& extension);

private:
    std::unordered_map<std::string, std::unique_ptr<Handler>> m_handlers;
};

template<typename THandler>
void register_handler() {
    ResourceRegistry::instance()->add<THandler>();
}

template<typename T>
HandlerT<T>::HandlerT(const char* tag)
    : m_tag(tag) {
}

template<typename T>
const std::string& HandlerT<T>::tag() const {
    return m_tag;
}

template<typename T>
Handle<void> HandlerT<T>::load(duk::tools::Globals* globals, Pools& pools, const Id& id, const std::filesystem::path& path, LoadMode loadMode) {
    std::vector<uint8_t> memory;
    switch (loadMode) {
        case LoadMode::UNPACKED:
            memory = duk::tools::load_bytes(path);
            break;
        case LoadMode::PACKED:
            memory = duk::tools::load_compressed_bytes(path);
            break;
    }

    auto resource = load_from_memory(globals, memory.data(), memory.size());

    return pools.insert(id, std::move(resource));
}

template<typename T>
void HandlerT<T>::solve_dependencies(const Handle<void>& handle, std::set<Id>& dependencies) {
    DependencySolver dependencySolver;
    dependencySolver.solve(*handle.as<T>());
    dependencies = dependencySolver.dependencies();
}

template<typename T>
void HandlerT<T>::solve_references(const Handle<void>& handle, Pools& pools) {
    ReferenceSolver referenceSolver(pools);
    referenceSolver.solve(*handle.as<T>());
}

template<typename T>
std::shared_ptr<T> TextHandlerT<T>::load_from_memory(duk::tools::Globals* globals, const void* data, size_t size) {
    const auto text = std::string(static_cast<const char*>(data), size);
    return load_from_text(globals, text);
}

template<typename THandler>
void ResourceRegistry::add() {
    static bool alreadyAdded = false;
    if (alreadyAdded) {
        return;
    }
    auto handler = std::make_unique<THandler>();
    m_handlers.emplace(handler->tag(), std::move(handler));
    alreadyAdded = true;
}

}// namespace duk::resource

#endif// DUK_RESOURCE_HANDLER_H
