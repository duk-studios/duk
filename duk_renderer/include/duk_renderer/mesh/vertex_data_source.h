//
// Created by Ricardo on 07/04/2023.
//

#ifndef DUK_RENDERER_VERTEX_DATA_SOURCE_H
#define DUK_RENDERER_VERTEX_DATA_SOURCE_H

#include <duk_renderer/data_source.h>
#include <duk_renderer/mesh/vertex_layout.h>

namespace duk::renderer {

class VertexDataSource : public DataSource {
protected:

    VertexDataSource(VertexAttribute::Format format);

protected:
    VertexAttribute::Format m_attributeFormat;
};

template<typename T>
class VertexDataSourceT : public VertexDataSource {
public:

    VertexDataSourceT();

    std::vector<T>& container();

private:
    std::vector<T> m_data;
};

template<typename T>
VertexDataSourceT<T>::VertexDataSourceT() :
    VertexDataSource(VertexAttribute::format_of<T>()){

}

template<typename T>
std::vector<T>& VertexDataSourceT<T>::container() {
    return m_data;
}


}

#endif //DUK_RENDERER_VERTEX_DATA_SOURCE_H
