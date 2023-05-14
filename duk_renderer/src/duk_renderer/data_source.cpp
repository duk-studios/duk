//
// Created by Ricardo on 07/04/2023.
//

#include <duk_renderer/data_source.h>

namespace duk::renderer {

renderer::DataSource::DataSource() :
    m_hash(UndefinedHash) {

}

renderer::DataSource::~DataSource() = default;

void DataSource::update_hash() {
    m_hash = calculate_hash();
}

Hash DataSource::hash() const {
    return m_hash;
}

}