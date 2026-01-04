//
// Created by Ricardo on 25/04/2024.
//

#ifndef DUK_ANIMATION_PROPERTY_H
#define DUK_ANIMATION_PROPERTY_H

#include <duk_objects/objects.h>

#include <duk_serial/json.h>

#include <duk_resource/solver/dependency_solver.h>
#include <duk_resource/solver/reference_solver.h>

#include <map>
#include <string>
#include <vector>
#include <unordered_map>

namespace duk::animation {

class Property {
public:
    virtual ~Property() = default;

    virtual void evaluate(const duk::objects::Object& object, uint32_t sample) const = 0;

    virtual uint32_t samples() const = 0;

    virtual const std::string& name() const = 0;
};

template<typename TEvaluator>
class PropertyT final : public Property {
public:
    using ValueType = typename TEvaluator::ValueType;

    void add_value(uint32_t sample, const ValueType& value);

    void evaluate(const duk::objects::Object& object, uint32_t sample) const override;

    const std::string& name() const override;

    ValueType sample_at(uint32_t sample) const;

    ValueType sample_interpolate(uint32_t sample) const;

    uint32_t samples() const override;

    typename std::vector<ValueType>::iterator begin();

    typename std::vector<ValueType>::iterator end();

    friend struct duk::serial::JsonPrimitiveValue<PropertyT<TEvaluator>>;

private:
    std::map<uint32_t, uint32_t> m_sampleToValueIndex;
    std::vector<ValueType> m_values;
};

class PropertyRegistry {
private:
    class PropertyEntry {
    public:
        virtual ~PropertyEntry() = default;

        virtual void json_write(rapidjson::Document& document, rapidjson::Value& json, const std::unique_ptr<Property>& property) const = 0;

        virtual void json_read(const rapidjson::Value& json, std::unique_ptr<Property>& property) const = 0;

        virtual void solve_resources(duk::resource::DependencySolver* solver, Property& property) const = 0;

        virtual void solve_resources(duk::resource::ReferenceSolver* solver, Property& property) const = 0;
    };

    template<typename TEvaluator>
    class PropertyEntryT : public PropertyEntry {
    public:
        void json_write(rapidjson::Document& document, rapidjson::Value& json, const std::unique_ptr<Property>& property) const override;

        void json_read(const rapidjson::Value& json, std::unique_ptr<Property>& property) const override;

        void solve_resources(duk::resource::DependencySolver* solver, Property& property) const override;

        void solve_resources(duk::resource::ReferenceSolver* solver, Property& property) const override;
    };

public:
    static PropertyRegistry* instance();

    void json_write(rapidjson::Document& document, rapidjson::Value& json, const std::unique_ptr<Property>& property) const;

    void json_read(const rapidjson::Value& json, std::unique_ptr<Property>& property) const;

    template<typename Solver>
    void solve_resources(Solver* solver, Property& property) const;

    template<typename T>
    void register_property();

private:
    PropertyEntry* find_entry(const std::string& type) const;

private:
    std::unordered_map<std::string, std::unique_ptr<PropertyEntry>> m_propertyEntries;
};

template<typename TEvaluator>
void register_property() {
    PropertyRegistry::instance()->register_property<TEvaluator>();
}

template<typename TEvaluator>
void PropertyT<TEvaluator>::add_value(uint32_t sample, const ValueType& value) {
    m_sampleToValueIndex.emplace(sample, static_cast<uint32_t>(m_values.size()));
    m_values.push_back(value);
}

template<typename TEvaluator>
void PropertyT<TEvaluator>::evaluate(const duk::objects::Object& object, uint32_t sample) const {
    TEvaluator::evaluate(object, this, sample);
}

template<typename TEvaluator>
const std::string& PropertyT<TEvaluator>::name() const {
    return duk::type::name_of<TEvaluator>();
}

template<typename TEvaluator>
typename PropertyT<TEvaluator>::ValueType PropertyT<TEvaluator>::sample_at(uint32_t sample) const {
    auto it = m_sampleToValueIndex.upper_bound(sample);
    if (it == m_sampleToValueIndex.end()) {
        return m_values.back();
    }
    if (it == m_sampleToValueIndex.begin()) {
        return m_values.front();
    }
    return m_values.at((--it)->second);
}

template<typename TEvaluator>
typename PropertyT<TEvaluator>::ValueType PropertyT<TEvaluator>::sample_interpolate(uint32_t sample) const {
    auto next = m_sampleToValueIndex.upper_bound(sample);
    if (next == m_sampleToValueIndex.end()) {
        return m_values.back();
    }
    if (next == m_sampleToValueIndex.begin()) {
        return m_values.front();
    }
    auto prev = next;
    --prev;
    const auto range = next->first - prev->first;
    const auto progress = (sample - prev->first) / static_cast<float>(range);
    return TEvaluator::interpolate(m_values.at(prev->second), m_values.at(next->second), progress);
}

template<typename TEvaluator>
uint32_t PropertyT<TEvaluator>::samples() const {
    return m_sampleToValueIndex.rbegin()->first;
}

template<typename TEvaluator>
typename std::vector<typename PropertyT<TEvaluator>::ValueType>::iterator PropertyT<TEvaluator>::begin() {
    return m_values.begin();
}

template<typename TEvaluator>
typename std::vector<typename PropertyT<TEvaluator>::ValueType>::iterator PropertyT<TEvaluator>::end() {
    return m_values.end();
}

template<typename TEvaluator>
void PropertyRegistry::PropertyEntryT<TEvaluator>::json_write(rapidjson::Document& document, rapidjson::Value& json, const std::unique_ptr<Property>& property) const {
    auto derivedProperty = dynamic_cast<const PropertyT<TEvaluator>*>(property.get());
    duk::serial::json_write_member_value(document, json, "values", *derivedProperty);
}

template<typename TEvaluator>
void PropertyRegistry::PropertyEntryT<TEvaluator>::json_read(const rapidjson::Value& json, std::unique_ptr<Property>& property) const {
    auto derivedProperty = std::make_unique<PropertyT<TEvaluator>>();
    duk::serial::json_read_member_value(json, "values", *derivedProperty);
    property = std::move(derivedProperty);
}

template<typename TEvaluator>
void PropertyRegistry::PropertyEntryT<TEvaluator>::solve_resources(duk::resource::DependencySolver* solver, Property& property) const {
    auto derivedProperty = dynamic_cast<PropertyT<TEvaluator>*>(&property);
    for (auto& value: *derivedProperty) {
        solver->solve(value);
    }
}

template<typename TEvaluator>
void PropertyRegistry::PropertyEntryT<TEvaluator>::solve_resources(duk::resource::ReferenceSolver* solver, Property& property) const {
    auto derivedProperty = dynamic_cast<PropertyT<TEvaluator>*>(&property);
    for (auto& value: *derivedProperty) {
        solver->solve(value);
    }
}

template<typename Solver>
void PropertyRegistry::solve_resources(Solver* solver, Property& property) const {
    const auto& name = property.name();
    auto entry = find_entry(name);
    entry->solve_resources(solver, property);
}

template<typename TEvaluator>
void PropertyRegistry::register_property() {
    const auto& type = duk::type::name_of<TEvaluator>();
    if (m_propertyEntries.contains(type)) {
        return;
    }
    m_propertyEntries.emplace(type, std::make_unique<PropertyEntryT<TEvaluator>>());
}

}// namespace duk::animation

namespace duk::serial {

// every property should be deserialized here, only their values need specific specializations
template<typename TEvaluator>
struct JsonPrimitiveValue<duk::animation::PropertyT<TEvaluator>> {

    static void write(rapidjson::Document& document, rapidjson::Value& json, const duk::animation::PropertyT<TEvaluator>& property);

    static void read(const rapidjson::Value& json, duk::animation::PropertyT<TEvaluator>& property);
};

template<>
struct JsonPrimitiveValue<std::unique_ptr<duk::animation::Property>> {

    static void write(rapidjson::Document& document, rapidjson::Value& json, const std::unique_ptr<duk::animation::Property>& value);

    static void read(const rapidjson::Value& json, std::unique_ptr<duk::animation::Property>& value);
};


template<typename TEvaluator>
void JsonPrimitiveValue<animation::PropertyT<TEvaluator>>::write(rapidjson::Document& document, rapidjson::Value& json, const animation::PropertyT<TEvaluator>& property) {
    auto jsonArray = json.SetArray().GetArray();
    for (const auto& [sample, valueIndex]: property.m_sampleToValueIndex) {
        rapidjson::Value jsonValue;
        jsonValue.SetObject();

        // this should be specialized for each property type
        const auto& value = property.m_values.at(valueIndex);
        duk::serial::json_write_member_value(document, jsonValue, "value", value);

        duk::serial::json_write_member_value(document, jsonValue, "sample", sample);

        jsonArray.PushBack(std::move(jsonValue), document.GetAllocator());
    }
}

template<typename TEvaluator>
void JsonPrimitiveValue<animation::PropertyT<TEvaluator>>::read(const rapidjson::Value& json, animation::PropertyT<TEvaluator>& property) {
    DUK_ASSERT(json.IsArray());
    for (const auto& jsonValue: json.GetArray()) {
        uint32_t sample;
        duk::serial::json_read_member_value(jsonValue, "sample", sample);

        // this should be specialized for each property type
        typename TEvaluator::ValueType value;
        duk::serial::json_read_member_value(jsonValue, "value", value);

        property.add_value(sample, value);
    }
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::animation::Property& property) {
    animation::PropertyRegistry::instance()->solve_resources(solver, property);
}

}// namespace duk::resource

#endif//DUK_ANIMATION_PROPERTY_H
