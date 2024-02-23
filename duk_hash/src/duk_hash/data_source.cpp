//
// Created by Ricardo on 07/04/2023.
//

#include <cassert>
#include <duk_hash/data_source.h>

namespace duk::hash {

DataSource::DataSource()
    : m_hash(hash::kUndefinedHash) {
}

DataSource::~DataSource() = default;

void DataSource::update_hash() {
    m_hash = calculate_hash();
}

hash::Hash DataSource::hash() const {
    assert(m_hash != hash::kUndefinedHash && "hash was not set, missing update_hash()");
    return m_hash;
}

}// namespace duk::hash