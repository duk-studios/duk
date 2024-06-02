//
// Created by Ricardo on 28/05/2024.
//

#ifndef DUK_RENDERER_STATIC_MESH_BUILDER_H
#define DUK_RENDERER_STATIC_MESH_BUILDER_H

#include <duk_renderer/mesh/static_mesh_data_source.h>

#include <glm/glm.hpp>

#include <span>

namespace duk::renderer {

static const std::set<VertexAttributes::Type> kDefaultVertexAttributes = {VertexAttributes::POSITION, VertexAttributes::NORMAL, VertexAttributes::UV};

bool calculate_normals(std::span<const glm::vec3> vertices, std::span<const uint32_t> indices, std::span<glm::vec3> normals);

bool calculate_normals(std::span<const glm::vec3> vertices, std::span<const uint16_t> indices, std::span<glm::vec3> normals);

bool calculate_normals(std::span<const glm::vec3> vertices, std::span<glm::vec3> normals);

StaticMeshDataSourceSOA build_quad_mesh_data(const std::set<VertexAttributes::Type>& attributes = kDefaultVertexAttributes, const glm::vec3& scale = glm::vec3(1.0f));

StaticMeshDataSourceSOA build_cube_mesh_data(const std::set<VertexAttributes::Type>& attributes = kDefaultVertexAttributes, const glm::vec3& scale = glm::vec3(1.0f));

StaticMeshDataSourceSOA build_sphere_mesh_data(uint32_t resolution, const std::set<VertexAttributes::Type>& attributes = kDefaultVertexAttributes, const glm::vec3& scale = glm::vec3(1.0f));

StaticMeshDataSourceSOA build_cone_mesh_data(uint32_t resolution, const std::set<VertexAttributes::Type>& attributes = kDefaultVertexAttributes, const glm::vec3& scale = glm::vec3(1.0f));

}// namespace duk::renderer

#endif//DUK_RENDERER_STATIC_MESH_BUILDER_H
