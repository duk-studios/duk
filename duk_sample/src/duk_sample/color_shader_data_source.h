/// 16/06/2023
/// color_shader_data_source.h

#ifndef DUK_SAMPLE_COLOR_SHADER_DATA_SOURCE_H
#define DUK_SAMPLE_COLOR_SHADER_DATA_SOURCE_H

#include <duk_rhi/pipeline/std_shader_data_source.h>

namespace duk::sample {

class ColorShaderDataSource : public duk::rhi::StdShaderDataSource {
public:
    ColorShaderDataSource();
};

}

#endif // DUK_SAMPLE_COLOR_SHADER_DATA_SOURCE_H

