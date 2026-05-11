/// pipeline_state_stack.h

#ifndef DUK_RHI_PIPELINE_STATE_STACK_H
#define DUK_RHI_PIPELINE_STATE_STACK_H

#include <duk_rhi/pipeline_state.h>

#include <stack>

namespace duk::rhi {

/// A simple stack of PipelineState values owned by CommandContext.
/// Callers push a copy of the current state, modify it freely, then pop to restore.
/// If the stack is empty when push() is called, a default-constructed state is pushed.
class PipelineStateStack {
public:
    /// Pushes a copy of the current top (or a default state if the stack is empty).
    void push();

    /// Pops the top state. Behaviour is undefined if the stack is empty.
    void pop();

    /// Returns a mutable reference to the top state.
    /// Behaviour is undefined if the stack is empty.
    PipelineState& top();

    /// Returns an immutable reference to the top state.
    /// Behaviour is undefined if the stack is empty.
    const PipelineState& top() const;

    bool empty() const;

private:
    std::stack<PipelineState> m_stack;
};

}// namespace duk::rhi

#endif// DUK_RHI_PIPELINE_STATE_STACK_H

