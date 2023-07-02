//
// Created by Ricardo on 07/04/2023.
//

#include <duk_rhi/data_source.h>
#include <cassert>

namespace duk::rhi {

rhi::DataSource::DataSource() :
    m_hash(hash::kUndefinedHash) {

}

rhi::DataSource::~DataSource() = default;

void DataSource::update_hash() {
    m_hash = calculate_hash();
}

hash::Hash DataSource::hash() const {
    assert(m_hash != hash::kUndefinedHash && "hash was not set, missing update_hash()");
    return m_hash;
}

}