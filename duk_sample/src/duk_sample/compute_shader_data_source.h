/// 16/06/2023
/// compute_shader_data_source.h

#ifndef DUK_SAMPLE_COMPUTE_SHADER_DATA_SOURCE_H
#define DUK_SAMPLE_COMPUTE_SHADER_DATA_SOURCE_H

#include <duk_renderer/pipeline/std_shader_data_source.h>

namespace duk::sample {

class ComputeShaderDataSource : public duk::renderer::StdShaderDataSource {
public:
    ComputeShaderDataSource();
};

}

#endif // DUK_SAMPLE_COMPUTE_SHADER_DATA_SOURCE_H

