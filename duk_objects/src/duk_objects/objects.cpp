/// 06/07/2023
/// objects.cpp

#include <duk_objects/objects.h>
#include <duk_objects/events.h>
#include <ranges>

namespace duk::objects {

namespace detail {

static bool json_component_has_data(const rapidjson::Value& json) {
    return json.IsObject() && !json.ObjectEmpty();
}

}// namespace detail

ComponentRegistry g_componentRegistry;

ComponentRegistry* ComponentRegistry::instance() {
    return &g_componentRegistry;
}

void ComponentRegistry::add_component(const ObjectHandle<false>& object, uint32_t componentId) {
    auto& entry = m_componentEntries.at(componentId);
    entry->add(object);
}

void ComponentRegistry::copy_component(const ObjectHandle<true>& src, ObjectHandle<false>& dst, uint32_t componentId) {
    auto& entry = m_componentEntries.at(componentId);
    entry->copy(src, dst);
}

void ComponentRegistry::json_read(const rapidjson::Value& json, ObjectHandle<false>& object, uint32_t componentId) {
    auto& entry = m_componentEntries.at(componentId);
    entry->json_read(json, object);
}

void ComponentRegistry::json_read(const rapidjson::Value& json, ObjectHandle<false>& object) {
    // read component type
    std::string type;
    duk::serial::json_read_member_value(json, "type", type);

    // read component data
    const auto it = m_componentNameToIndex.find(type);
    if (it == m_componentNameToIndex.end()) {
        duk::log::warn("Unregistered Component type: \"{}\"", type);
        return;
    }
    const auto index = it->second;
    if (const auto componentDataJsonIt = json.FindMember("data"); componentDataJsonIt != json.MemberEnd()) {
        const auto& componentDataJson = componentDataJsonIt->value;
        json_read(componentDataJson, object, index);
    } else {
        // no data, just add the component
        add_component(object, index);
    }
}

void ComponentRegistry::json_write(rapidjson::Document& document, rapidjson::Value& json, const ObjectHandle<true>& object, uint32_t componentIndex) {
    json.SetObject();
    // write component type
    const auto& type = name_of(componentIndex);
    duk::serial::json_write_member_value(document, json, "type", type);

    // write component data
    rapidjson::Value componentDataJson;
    m_componentEntries.at(componentIndex)->json_write(document, componentDataJson, object);
    if (detail::json_component_has_data(componentDataJson)) {
        json.AddMember("data", std::move(componentDataJson), document.GetAllocator());
    }
}

std::unique_ptr<detail::ComponentPool> ComponentRegistry::create_pool(uint32_t index) const {
    return m_componentEntries.at(index)->create_pool();
}

const std::string& ComponentRegistry::name_of(uint32_t index) const {
    DUK_ASSERT(m_componentEntries[index] != nullptr);
    return m_componentEntries[index]->name();
}

uint32_t ComponentRegistry::index_of(const std::string& componentTypeName) const {
    return m_componentNameToIndex.at(componentTypeName);
}

ObjectSolver::ObjectSolver(Objects& objects)
    : m_objects(objects)
    , m_runtimeIds(nullptr) {
}

ObjectSolver::ObjectSolver(Objects& objects, const std::unordered_map<Id, Id>& runtimeIds)
    : m_objects(objects)
    , m_runtimeIds(&runtimeIds) {
}

void ObjectSolver::solve(Id& id) {
    id = find_runtime_id(id);
}

Id ObjectSolver::find_runtime_id(Id originalId) const {
    if (originalId.m_resourceId != resource::kInvalidId) {
        originalId = m_objects.find_object_id(originalId.m_resourceId);
    } else if (m_runtimeIds) {
        if (const auto it = m_runtimeIds->find(originalId); it != m_runtimeIds->end()) {
            originalId = it->second;
        }
    }
    return originalId;
}

Objects::Objects()
    : m_dirty(false) {
}

Objects::~Objects() {
    for (auto object: all(true)) {
        object.destroy();
    }

    for (int index = 0; index < m_exitComponentMasks.size(); index++) {
        auto& componentMask = m_exitComponentMasks[index];
        for (auto componentIndex: componentMask.bits<true>()) {
            m_componentPools[componentIndex]->destruct(index);
            componentMask.reset(componentIndex);
        }
    }
}

ObjectHandle<false> Objects::add_object() {
    m_dirty = true;
    if (!m_freeList.empty()) {
        auto freeIndex = m_freeList.back();
        m_freeList.pop_back();
        auto version = m_versions[freeIndex];
        m_enterComponentMasks[freeIndex].reset();
        m_activeComponentMasks[freeIndex].reset();
        m_exitComponentMasks[freeIndex].reset();
        m_ids[freeIndex] = duk::resource::Id();
        m_parentIndices[freeIndex] = kInvalidObjectIndex;
        m_nodes[freeIndex] = {.self = freeIndex};
        m_enterIndices[freeIndex] = true;
        return {freeIndex, version, this};
    }
    auto index = static_cast<uint32_t>(m_versions.size());
    m_versions.resize(m_versions.size() + 1);
    m_enterComponentMasks.resize(m_versions.size());
    m_activeComponentMasks.resize(m_versions.size());
    m_exitComponentMasks.resize(m_versions.size());
    m_enterIndices.resize(m_versions.size());
    m_exitIndices.resize(m_versions.size());
    m_parentIndices.resize(m_versions.size());
    m_parentIndices[index] = kInvalidObjectIndex;
    m_ids.resize(m_versions.size());
    m_nodes.resize(m_versions.size());
    m_nodes[index].self = index;
    m_versions[index] = 0;
    m_enterIndices[index] = true;
    return {index, 0, this};
}

ObjectHandle<false> Objects::add_object(const Id& parent) {
    const auto object = add_object();
    // only connect this object to it's parent, the rest of the hierarchy will be resolved on update
    m_parentIndices[object.id().index()] = parent.index();
    return object;
}

ObjectHandle<false> Objects::copy_object(const ObjectHandle<true>& src, const Id& parent) {
    auto dst = add_object(parent);
    auto originalComponentMask = src.component_mask();
    for (auto componentIndex: originalComponentMask.bits<true>()) {
        ComponentRegistry::instance()->copy_component(src, dst, componentIndex);
    }
    return dst;
}

ObjectHandle<false> Objects::copy_objects(const Objects& src, const Id& parent) {
    ObjectHandle<false> root;
    std::unordered_map<Id, Id> runtimeIds;
    for (const auto srcObj: src.all(true)) {
        const auto dstObj = copy_object(srcObj, srcObj.parent().id());
        if (!root.valid()) {
            root = dstObj;
        }
        runtimeIds[srcObj.id()] = dstObj.id();
    }

    // resolve object hierarchy and component references
    ObjectSolver solver(*this, runtimeIds);
    for (auto runtimeId: runtimeIds | std::views::values) {
        auto runtimeObj = object(runtimeId);

        auto index = runtimeId.index();
        // resolve parent and children
        auto parentIndex = m_parentIndices[index];

        // if object was at root, place it as a child of the provided parent
        if (parentIndex == kInvalidObjectIndex) {
            parentIndex = parent.index();
        } else {
            Id id(parentIndex, 0);
            solver.solve(id);
            parentIndex = id.index();
        }
        m_parentIndices[index] = parentIndex;

        auto componentMask = runtimeObj.component_mask();
        for (auto componentIndex: componentMask.bits<true>()) {
            ComponentRegistry::instance()->solve(&solver, runtimeObj, componentIndex);
        }
    }
    return root;
}

void Objects::destroy_object(const Id& id) {
    const auto index = id.index();
    if (m_exitIndices[index]) {
        return;
    }
    auto& node = m_nodes[index];
    auto childIndex = node.child;
    while (childIndex != kInvalidObjectIndex) {
        auto childId = Id(childIndex, m_versions[childIndex]);
        destroy_object(childId);
        childIndex = m_nodes[childIndex].next;
    }
    m_exitIndices[index] = true;
    m_enterIndices[index] = false;
    m_exitComponentMasks[index] = m_activeComponentMasks[index];
    m_activeComponentMasks[index].reset();
    m_dirty = true;
}

ObjectHandle<false> Objects::object(const Id& id) {
    return {id.index(), id.version(), this};
}

ObjectHandle<true> Objects::object(const Id& id) const {
    return {id.index(), id.version(), this};
}

ObjectHandle<false> Objects::parent(const Id& id) {
    const auto parentIndex = m_parentIndices[id.index()];
    if (parentIndex == kInvalidObjectIndex) {
        return {kInvalidObjectIndex, 0, this};
    }
    return {parentIndex, m_versions[parentIndex], this};
}

ObjectHandle<true> Objects::parent(const Id& id) const {
    const auto parentIndex = m_parentIndices[id.index()];
    if (parentIndex == kInvalidObjectIndex) {
        return {kInvalidObjectIndex, 0, this};
    }
    return {parentIndex, m_versions[parentIndex], this};
}

resource::Id Objects::resource_id(const Id& id) const {
    if (!valid_object(id)) {
        return resource::kInvalidId;
    }
    return m_ids[id.index()];
}

void Objects::reparent(const Id& id, const Id& parent) {
    DUK_ASSERT(valid_object(id));
    DUK_ASSERT(valid_object(parent) || parent.index() == kInvalidObjectIndex);
    const auto index = id.index();

    remove_node(index);

    // set new parent index
    m_parentIndices[index] = parent.index();

    add_node(index);
}

bool Objects::valid_object(const Id& id) const {
    const auto index = id.index();
    return m_versions.size() > index && m_versions[index] == id.version();
}

uint32_t Objects::count() const {
    return static_cast<uint32_t>(m_versions.size() - m_freeList.size());
}

Objects::ObjectView<false> Objects::all(bool includeInactive) {
    return ObjectView<false>(this, {}, includeInactive);
}

Objects::ObjectView<true> Objects::all(bool includeInactive) const {
    return ObjectView<true>(this, {}, includeInactive);
}

ComponentMask Objects::component_mask(const Id& id) const {
    const auto index = id.index();
    return m_activeComponentMasks.at(index) | m_exitComponentMasks.at(index);
}

void Objects::update(ComponentEventDispatcher& dispatcher) {
    if (!m_dirty) {
        return;
    }
    m_dirty = false;

    // swap with clean data to make sure that we don't iterate over modified vectors
    std::vector<ComponentMask> enterComponentMasks(m_enterComponentMasks.size());
    std::vector<bool> enterIndices(m_enterIndices.size());
    std::vector<bool> exitIndices(m_exitIndices.size());

    std::swap(enterComponentMasks, m_enterComponentMasks);
    std::swap(enterIndices, m_enterIndices);
    std::swap(exitIndices, m_exitIndices);

    // update node tree
    for (auto index = 0; index < enterIndices.size(); index++) {
        if (enterIndices[index]) {
            enterIndices[index] = false;
            add_node(index);
        }
    }

    // we need to while iterate, because new ids might be added during event dispatching
    for (auto index = 0; index < enterComponentMasks.size(); index++) {
        auto& enterComponentMask = enterComponentMasks[index];
        for (auto componentIndex: enterComponentMask.bits<true>()) {
            enterComponentMask.reset(componentIndex);
            dispatcher.emit_component<ComponentEnterEvent>(object(Id(index, m_versions[index])), componentIndex);
        }
    }
    for (auto index = 0; index < m_exitComponentMasks.size(); index++) {
        auto& exitComponentMask = m_exitComponentMasks[index];
        for (auto componentIndex: exitComponentMask.bits<true>()) {
            dispatcher.emit_component<ComponentExitEvent>(object(Id(index, m_versions[index])), componentIndex);
            exitComponentMask.reset(componentIndex);
            m_componentPools[componentIndex]->destruct(index);
        }
    }
    bool needsSort = false;
    for (auto index = 0; index < exitIndices.size(); index++) {
        if (exitIndices[index]) {
            exitIndices[index] = false;
            remove_node(index);
            m_versions[index]++;
            m_freeList.push_back(index);
            needsSort = true;
        }
    }
    if (needsSort) {
        std::ranges::sort(m_freeList);
    }
}

void Objects::add_component(uint32_t index, uint32_t componentIndex) {
    m_enterComponentMasks[index].set(componentIndex);
    m_activeComponentMasks[index].set(componentIndex);
    auto& pool = m_componentPools[componentIndex];
    if (!pool) {
        pool = ComponentRegistry::instance()->create_pool(componentIndex);
    }
    pool->construct(index);
    m_dirty = true;
}

void Objects::remove_component(uint32_t index, uint32_t componentIndex) {
    m_exitComponentMasks[index].set(componentIndex);
    m_activeComponentMasks[index].reset(componentIndex);
    m_dirty = true;
}

void Objects::add_node(uint32_t nodeIndex) {
    auto& node = m_nodes[nodeIndex];
    auto& parent = parent_node(nodeIndex);

    if (parent.child == kInvalidObjectIndex) {
        parent.child = node.self;
        return;
    }

    uint32_t siblingIndex = parent.child;
    while (siblingIndex != kInvalidObjectIndex) {
        const auto& sibling = m_nodes[siblingIndex];
        if (sibling.next == kInvalidObjectIndex) {
            break;
        }
        siblingIndex = sibling.next;
    }

    auto& sibling = m_nodes[siblingIndex];
    sibling.next = node.self;

    node.previous = siblingIndex;
}

void Objects::remove_node(uint32_t nodeIndex) {
    auto& node = m_nodes[nodeIndex];
    auto& parent = parent_node(nodeIndex);
    if (parent.child == node.self) {
        parent.child = node.next;
    }
    if (node.previous != kInvalidObjectIndex) {
        auto& previous = m_nodes[node.previous];
        previous.next = node.next;
    }
    if (node.next != kInvalidObjectIndex) {
        auto& next = m_nodes[node.next];
        next.previous = node.previous;
    }
    m_parentIndices[nodeIndex] = kInvalidObjectIndex;
    node = {};
}

Objects::Node& Objects::parent_node(uint32_t nodeIndex) {
    return m_parentIndices[nodeIndex] != kInvalidObjectIndex ? m_nodes[m_parentIndices[nodeIndex]] : m_root;
}

ObjectHandle<false> Objects::add_object_with_id(const duk::resource::Id& id) {
    const auto object = add_object();
    m_ids[object.id().index()] = id;
    return object;
}

Id Objects::find_object_id(const resource::Id& resourceId) const {
    Id result;
    for (uint32_t i = 0; i < m_ids.size(); ++i) {
        if (m_ids[i] == resourceId) {
            result = Id(i, m_versions[i]);
            break;
        }
    }
    return result;
}

ComponentEventListener::ComponentEventListener()
    : m_dispatcher(nullptr) {
}

void ComponentEventListener::attach(ComponentEventDispatcher* dispatcher) {
    if (m_dispatcher == dispatcher) {
        return;
    }
    m_listener.clear();
    m_dispatcher = dispatcher;
}

}// namespace duk::objects

namespace duk::serial {

static duk::resource::Id generate_unique_id(const std::vector<duk::resource::Id>& existingIds) {
    duk::resource::Id newId;
    do {
        newId = resource::generate_id();
    } while (std::ranges::find(existingIds, newId) != existingIds.end());
    return newId;
}

static void generate_ids(const duk::objects::Objects& objects, std::vector<duk::resource::Id>& ids) {
    for (auto object: objects.all(true)) {
        auto index = object.id().index();
        if (ids[index] != resource::kInvalidId) {
            continue;
        }
        ids[index] = generate_unique_id(ids);
    }
}

void JsonPrimitiveValue<duk::objects::Objects>::write(rapidjson::Document& document, rapidjson::Value& json, const duk::objects::Objects& objects) {
    auto jsonArray = json.SetArray().GetArray();
    auto componentRegistry = objects::ComponentRegistry::instance();

    auto& ids = objects.m_ids;
    // generate unique ids for all objects
    generate_ids(objects, ids);

    for (auto object: objects.all(true)) {
        rapidjson::Value jsonElement;
        jsonElement.SetObject();

        const auto index = object.id().index();
        json_write_member_value(document, jsonElement, "id", ids[index]);

        const auto parentIndex = objects.m_parentIndices[index];
        const auto parentId = parentIndex != objects::kInvalidObjectIndex ? ids[parentIndex] : resource::kInvalidId;
        json_write_member_value(document, jsonElement, "parent", parentId);
        {
            rapidjson::Value jsonComponents;
            auto jsonComponentsArray = jsonComponents.SetArray().GetArray();
            auto mask = object.component_mask();
            for (auto componentIndex: mask.bits<true>()) {
                rapidjson::Value jsonComponent;
                componentRegistry->json_write(document, jsonComponent, object, componentIndex);
                jsonComponentsArray.PushBack(jsonComponent, document.GetAllocator());
            }
            jsonElement.AddMember("components", std::move(jsonComponents), document.GetAllocator());
        }
        jsonArray.PushBack(std::move(jsonElement), document.GetAllocator());
    }
}

void JsonPrimitiveValue<duk::objects::Objects>::read(const rapidjson::Value& json, duk::objects::Objects& objects) {
    DUK_ASSERT(json.IsArray());
    auto componentRegistry = objects::ComponentRegistry::instance();
    auto jsonArray = json.GetArray();
    std::unordered_map<duk::objects::Id, duk::resource::Id> objectToParent;
    for (auto& jsonElement: jsonArray) {
        DUK_ASSERT(jsonElement.IsObject());

        duk::resource::Id id;
        json_read_member_value(jsonElement, "id", id);

        duk::resource::Id parentId;
        json_read_member_value(jsonElement, "parent", parentId);

        auto object = objects.add_object_with_id(id);

        objectToParent[object.id()] = parentId;

        auto jsonComponentsArray = jsonElement["components"].GetArray();
        for (auto& jsonComponent: jsonComponentsArray) {
            componentRegistry->json_read(jsonComponent, object);
        }
    }

    duk::objects::ObjectSolver solver(objects);
    for (auto object: objects.all(true)) {
        // fix up parent ids to the correct runtime id
        const auto parentResourceId = objectToParent[object.id()];
        const auto parentId = objects.find_object_id(parentResourceId);
        objects.m_parentIndices[object.id().index()] = parentId.index();
        auto componentMask = object.component_mask();
        for (auto componentIndex: componentMask.bits<true>()) {
            objects::ComponentRegistry::instance()->solve(&solver, object, componentIndex);
        }
    }
}

}// namespace duk::serial
