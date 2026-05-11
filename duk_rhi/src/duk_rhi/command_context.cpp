/// command_context.cpp

#include <duk_rhi/command_context.h>

namespace duk::rhi {

CommandContext::~CommandContext() = default;

PipelineStateStack& CommandContext::pipeline_state_stack() {
    return m_pipelineStateStack;
}

}// namespace duk::rhi

