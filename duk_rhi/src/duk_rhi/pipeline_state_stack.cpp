/// pipeline_state_stack.cpp

#include <duk_rhi/pipeline_state_stack.h>

#include <duk_macros/assert.h>

namespace duk::rhi {

void PipelineStateStack::push() {
    if (m_stack.empty()) {
        m_stack.emplace();
    } else {
        m_stack.push(m_stack.top());
    }
}

void PipelineStateStack::pop() {
    DUK_ASSERT(!m_stack.empty());
    m_stack.pop();
}

PipelineState& PipelineStateStack::top() {
    DUK_ASSERT(!m_stack.empty());
    return m_stack.top();
}

const PipelineState& PipelineStateStack::top() const {
    DUK_ASSERT(!m_stack.empty());
    return m_stack.top();
}

bool PipelineStateStack::empty() const {
    return m_stack.empty();
}

}// namespace duk::rhi
