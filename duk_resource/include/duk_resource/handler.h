/// 13/11/2023
/// handler.h

#ifndef DUK_RESOURCE_HANDLER_H
#define DUK_RESOURCE_HANDLER_H

#include <duk_resource/file.h>
#include <duk_resource/pool.h>
#include <duk_resource/handle.h>
#include <duk_resource/set.h>
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

    virtual Handle<void> load(Pools* pools, const Id& id, const std::filesystem::path& path, LoadMode loadMode) = 0;

    virtual void solve_dependencies(const Handle<void>& handle, std::set<Id>& dependencies) = 0;

    virtual void solve_references(const Handle<void>& handle, Pools* pools) = 0;
};

template<typename TPool>
class HandlerT : public Handler {
public:
    using Type = typename TPool::Type;

    explicit HandlerT(const char* tag);

    const std::string& tag() const final;

    Handle<void> load(Pools* pools, const Id& id, const std::filesystem::path& path, LoadMode loadMode) override;

    void solve_dependencies(const Handle<void>& handle, std::set<Id>& dependencies) override;

    void solve_references(const Handle<void>& handle, Pools* pools) override;

protected:
    virtual Handle<Type> load_from_memory(TPool* pool, const Id& id, const void* data, size_t size) = 0;

private:
    static TPool* find_pool(Pools* pools);

    std::string m_tag;
};

template<typename TPool>
class TextHandlerT : public HandlerT<TPool> {
public:
    using Type = typename TPool::Type;

    using HandlerT<TPool>::HandlerT;

protected:
    Handle<Type> load_from_memory(TPool* pool, const Id& id, const void* data, size_t size) override;

    virtual Handle<Type> load_from_text(TPool* pool, const Id& id, const std::string_view& text) = 0;
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

template<typename TPool>
HandlerT<TPool>::HandlerT(const char* tag)
    : m_tag(tag) {
}

template<typename TPool>
const std::string& HandlerT<TPool>::tag() const {
    return m_tag;
}

template<typename TPool>
Handle<void> HandlerT<TPool>::load(Pools* pools, const Id& id, const std::filesystem::path& path, LoadMode loadMode) {
    std::vector<uint8_t> memory;
    switch (loadMode) {
        case LoadMode::UNPACKED:
            memory = duk::tools::load_bytes(path);
            break;
        case LoadMode::PACKED:
            memory = duk::tools::load_compressed_bytes(path);
            break;
    }
    auto pool = find_pool(pools);
    return load_from_memory(pool, id, memory.data(), memory.size());
}

template<typename TPool>
void HandlerT<TPool>::solve_dependencies(const Handle<void>& handle, std::set<Id>& dependencies) {
    DependencySolver dependencySolver;
    dependencySolver.solve(*handle.as<Type>());
    dependencies = dependencySolver.dependencies();
}

template<typename TPool>
void HandlerT<TPool>::solve_references(const Handle<void>& handle, Pools* pools) {
    ReferenceSolver referenceSolver(pools);
    referenceSolver.solve(*handle.as<Type>());
}

template<typename TPool>
TPool* HandlerT<TPool>::find_pool(Pools* pools) {
    auto pool = pools->get<TPool>();
    if (!pool) {
        throw std::logic_error("Failed to find valid pool for HandlerT");
    }
    return pool;
}

template<typename TPool>
Handle<typename TextHandlerT<TPool>::Type> TextHandlerT<TPool>::load_from_memory(TPool* pool, const Id& id, const void* data, size_t size) {
    const auto text = std::string(static_cast<const char*>(data), size);
    return load_from_text(pool, id, text);
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
