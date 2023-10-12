/// 05/10/2023
/// pass.cpp

#include <duk_renderer/passes/pass.h>

namespace duk::renderer {

Pass::~Pass() = default;

void PassConnection::update(duk::rhi::Image* image) {
    m_image = image;
    for (auto& connection : m_connections) {
        connection->update(image);
    }
}

void PassConnection::connect(PassConnection* connection) {
    m_connections.push_back(connection);
}

duk::rhi::Image* PassConnection::image() const {
    return m_image;
}

}