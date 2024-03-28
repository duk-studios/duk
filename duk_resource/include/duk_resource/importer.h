/// 13/11/2023
/// importer.h

#ifndef DUK_RESOURCE_IMPORTER_H
#define DUK_RESOURCE_IMPORTER_H

#include <duk_resource/pool.h>
#include <duk_resource/resource.h>
#include <duk_resource/solver/dependency_solver.h>
#include <duk_resource/solver/reference_solver.h>

#include <filesystem>

namespace duk::resource {

// 1,000,000 reserved for built-in resources
static constexpr Id kMaxBuiltInResourceId(1000000);

struct ImporterCreateInfo {
    Pools* pools;
};

class ResourceImporter {
public:
    virtual ~ResourceImporter() = default;

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

class Importer {
public:
    explicit Importer(const ImporterCreateInfo& importerCreateInfo);

    ~Importer();

    ResourceImporter* add_resource_importer(std::unique_ptr<ResourceImporter> resourceImporter);

    template<typename T, typename... Args>
    T* add_resource_importer(Args&&... args);

    void load_resource_set(const std::filesystem::path& path);

    void load_resource(Id id);

    Id find_id(const std::string& alias);

    ResourceImporter* get_importer(const std::string& tag);

    template<typename T>
    T* get_importer_as(const std::string& tag);

private:
    Pools* m_pools;
    std::unordered_map<std::string, std::unique_ptr<ResourceImporter>> m_resourceImporters;
    ResourceSet m_resourceSet;
};

template<typename T, typename... Args>
T* Importer::add_resource_importer(Args&&... args) {
    return dynamic_cast<T*>(add_resource_importer(std::make_unique<T>(std::forward<Args>(args)...)));
}

template<typename T>
T* Importer::get_importer_as(const std::string& tag) {
    auto importer = get_importer(tag);
    if (importer->tag() != tag) {
        return nullptr;
    }
    return dynamic_cast<T*>(importer);
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

#endif// DUK_RESOURCE_IMPORTER_H
