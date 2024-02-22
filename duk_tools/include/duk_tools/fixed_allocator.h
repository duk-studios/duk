/// 06/05/2023
/// fixed_allocator.h

#ifndef DUK_TOOLS_FIXED_ALLOCATOR_H
#define DUK_TOOLS_FIXED_ALLOCATOR_H

#include <duk_tools/interval_map.h>

#include <cstdint>
#include <cstdlib>

namespace duk::tools {
template<typename T, size_t N>
class FixedAllocator {
public:
    // Required typedefs for allocator
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    static constexpr const_pointer kInvalidPointer = ~0;

    // Constructor and destructor
    FixedAllocator() : m_block(malloc(sizeof(value_type) * N)),
                       m_allocationMap(kInvalidPointer) {
        // nullptr indicates a free block
        m_allocationMap.assign(m_block, m_block + N, nullptr);
    }

    ~FixedAllocator() {
        free(m_block);
    }

    // Allocate and deallocate methods
    pointer allocate(size_type n) {
        auto ptr = find_free_block(n);
        if (!ptr) {
            return ptr;
        }

        m_allocationMap.assign(ptr, ptr + n, ptr);

        return ptr;
    }

    void deallocate(pointer ptr, size_type n) {
        m_allocationMap.assign(ptr, ptr + n, nullptr);
    }

    // Construct and destroy methods
    template<typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        ::new ((void*)p) U(std::forward<Args>(args)...);
    }

    template<typename U>
    void destroy(U* p) {
        p->~U();
    }

    // Copy and move constructors
    FixedAllocator(const FixedAllocator&) = default;
    FixedAllocator(FixedAllocator&&) = default;

    // Copy and move assignment operators
    FixedAllocator& operator=(const FixedAllocator&) = default;
    FixedAllocator& operator=(FixedAllocator&&) = default;

    // Rebind for different types
    template<typename U>
    struct rebind {
        using other = FixedAllocator<U, N>;
    };

private:
    pointer find_free_block(size_t minimumSize) {
        pointer startPtr = m_block;
        pointer endPtr = startPtr + N;

        while (startPtr < endPtr) {
            startPtr = m_allocationMap.find_first_of(nullptr, startPtr, endPtr);

            auto blockSize = m_allocationMap.interval_size(startPtr);
            if (blockSize >= minimumSize) {
                return startPtr;
            }
        }
        return nullptr;
    }

private:
    pointer m_block;
    IntervalMap<pointer, pointer> m_allocationMap;
};

}// namespace duk::tools

#endif// DUK_TOOLS_FIXED_ALLOCATOR_H
