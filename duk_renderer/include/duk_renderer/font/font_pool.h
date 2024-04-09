//
// Created by Ricardo on 29/03/2024.
//

#ifndef DUK_RENDERER_FONT_POOL_H
#define DUK_RENDERER_FONT_POOL_H

#include <duk_renderer/font/font.h>

#include <duk_resource/pool.h>

namespace duk::renderer {

class FontPool : public duk::resource::PoolT<FontResource> {};

}// namespace duk::renderer

#endif//DUK_RENDERER_FONT_POOL_H
