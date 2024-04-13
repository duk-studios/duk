/// 13/11/2023
/// handler.h

#ifndef DUK_RESOURCE_HANDLER_H
#define DUK_RESOURCE_HANDLER_H

#include <duk_resource/pool.h>
#include <duk_resource/resource.h>
#include <duk_resource/solver/dependency_solver.h>
#include <duk_resource/solver/reference_solver.h>

#include <filesystem>

namespace duk::resource {

// 1,000,000 reserved for built-in resources
static constexpr Id kMaxBuiltInResourceId(1000000);

struct HandlerCreateInfo {
    Pools* pools;
};

class ResourceHandler {
public:
    virtual ~ResourceHandler() = default;

    virtual const std::string& tag() const = 0;

    virtual void load(const Id& id, const std::filesystem::path& path) = 0;

    virtual void solve_dependencies(const Id& id, DependencySolver& dependencySolver);

    virtual void solve_references(const Id& id, ReferenceSolver& referenceSolver);
};

struct ResourceDescription {
    std::string tag;
    std::string file;
    Id id;
    std::set<std::string> aliases;
};

bool operator==(const ResourceDescription& lhs, const ResourceDescription& rhs);

bool operator<(const ResourceDescription& lhs, const ResourceDescription& rhs);

struct ResourceSet {
    std::unordered_map<duk::resource::Id, ResourceDescription> resources;
    std::unordered_map<std::string, duk::resource::Id> aliases;
};

class Handler {
public:
    explicit Handler(const HandlerCreateInfo& handlerCreateInfo);

    ~Handler();

    ResourceHandler* add_resource_handler(std::unique_ptr<ResourceHandler> resourceHandler);

    template<typename T, typename... Args>
    T* add_resource_handler(Args&&... args);

    void load_resource_set(const std::filesystem::path& path);

    void load_resource(Id id);

    Id find_id(const std::string& alias);

    ResourceHandler* get_handler(const std::string& tag);

    template<typename T>
    T* get_handler_as(const std::string& tag);

private:
    Pools* m_pools;
    std::unordered_map<std::string, std::unique_ptr<ResourceHandler>> m_resourceHandlers;
    ResourceSet m_resourceSet;
};

template<typename T, typename... Args>
T* Handler::add_resource_handler(Args&&... args) {
    return dynamic_cast<T*>(add_resource_handler(std::make_unique<T>(std::forward<Args>(args)...)));
}

template<typename T>
T* Handler::get_handler_as(const std::string& tag) {
    auto handler = get_handler(tag);
    if (handler->tag() != tag) {
        return nullptr;
    }
    return dynamic_cast<T*>(handler);
}

}// namespace duk::resource

namespace duk::serial {

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::resource::ResourceDescription& resourceDescription) {
    visitor->visit_member(resourceDescription.tag, "tag");
    visitor->visit_member(resourceDescription.id, "id");
    visitor->visit_member(resourceDescription.file, "file");
    visitor->visit_member_array(resourceDescription.aliases, "aliases");
}

}// namespace duk::serial

#endif// DUK_RESOURCE_HANDLER_H
