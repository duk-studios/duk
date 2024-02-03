/// 12/09/2023
/// material.h

#ifndef DUK_RENDERER_MATERIAL_H
#define DUK_RENDERER_MATERIAL_H

#include <duk_renderer/renderer.h>
#include <duk_renderer/resources/materials/draw_entry.h>
#include <duk_renderer/resources/materials/painter.h>

#include <duk_resource/resource.h>
#include <duk_rhi/command/command_buffer.h>
#include <duk_scene/object.h>

namespace duk::renderer {

class GlobalDescriptors;

enum class MaterialType {
    UNDEFINED = 0,
    COLOR,
    PHONG,
    FULLSCREEN,
    SPRITE_COLOR,
};

class MaterialDataSource : public duk::hash::DataSource {
protected:

    explicit MaterialDataSource(MaterialType type);

public:

    DUK_NO_DISCARD MaterialType type() const;

    template<typename T>
    T* as() requires std::is_base_of_v<MaterialDataSource, T>;

    template<typename T>
    const T* as() const requires std::is_base_of_v<MaterialDataSource, T>;

private:
    MaterialType m_type;
};

class Material {
protected:

    Material(Renderer* renderer, const duk::rhi::ShaderDataSource* shaderDataSource);

    virtual ~Material();

public:

    DUK_NO_DISCARD Painter* painter();

    DUK_NO_DISCARD const Painter* painter() const;

private:
    Painter m_painter;
};

using MaterialResource = duk::resource::ResourceT<Material>;

template<typename T>
T* MaterialDataSource::as() requires std::is_base_of_v<MaterialDataSource, T> {
    return dynamic_cast<T*>(this);
}

template<typename T>
const T* MaterialDataSource::as() const requires std::is_base_of_v<MaterialDataSource, T> {
    return dynamic_cast<const T*>(this);
}

}

#endif // DUK_RENDERER_MATERIAL_H

