//
// Created by Ricardo on 07/04/2023.
//

#include <duk_renderer/data_source.h>
#include <duk_macros/assert.h>

namespace duk::renderer {

renderer::DataSource::DataSource() :
    m_hash(hash::UndefinedHash) {

}

renderer::DataSource::~DataSource() = default;

void DataSource::update_hash() {
    m_hash = calculate_hash();
}

hash::Hash DataSource::hash() const {
    DUK_ASSERT(m_hash != hash::UndefinedHash, "hash was not set, missing update_hash()");
    return m_hash;
}

}