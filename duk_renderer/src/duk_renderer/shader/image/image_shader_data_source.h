#ifndef DUK_RENDERER_IMAGE_SHADER_DATA_SOURCE_H
#define DUK_RENDERER_IMAGE_SHADER_DATA_SOURCE_H

#include <vector>
#include <cstdint>

namespace duk::renderer {

extern std::vector<uint8_t> image_vert_spir_v();

extern std::vector<uint8_t> image_frag_spir_v();

}// namespace duk::renderer

#endif// DUK_RENDERER_IMAGE_SHADER_DATA_SOURCE_H
