/// 05/10/2023
/// pass.cpp

#include <duk_renderer/passes/pass.h>

#include <algorithm>
#include <cassert>

namespace duk::renderer {

PassConnection::PassConnection() :
    m_image(nullptr),
    m_parent(nullptr) {

}

PassConnection::~PassConnection() {
    for (auto child : m_children) {
        child->m_parent = nullptr;
    }
}

void PassConnection::update(duk::rhi::Image* image) {
    m_image = image;
    for (auto& child : m_children) {
        child->update(image);
    }
}

void PassConnection::connect(PassConnection* connection) {
    auto previousParent = connection->m_parent;
    if (previousParent) {
        // we need to disconnect the previous connection
        previousParent->disconnect(connection);
    }

    m_children.push_back(connection);
    connection->m_parent = this;
}

void PassConnection::disconnect(PassConnection* connection) {
    assert(connection->m_parent == this);
    auto it = std::find(m_children.begin(), m_children.end(), connection);
    m_children.erase(it);
    connection->m_parent = nullptr;
}

duk::rhi::Image* PassConnection::image() const {
    return m_image;
}

Pass::~Pass() = default;


}