/// 09/07/2023
/// bit_block.h

#ifndef DUK_TOOLS_BIT_BLOCK_H
#define DUK_TOOLS_BIT_BLOCK_H

#include <cstdint>
#include <array>
#include <algorithm>
#include <limits>
#include <bit>
#include <string>
#include <sstream>

namespace duk::tools {

template<size_t N>
class BitBlock {
public:
    using Block = std::conditional_t<(N > 32), uint64_t, std::conditional_t<(N > 16), uint32_t, std::conditional_t<(N > 8), uint16_t, uint8_t>>>;
    static constexpr size_t kBlockCount = (63 + N) / 64;
    static constexpr size_t kBlockBitCount = sizeof(Block) * 8;
    static constexpr size_t kLastBlockRemainder = N % kBlockBitCount;
    static constexpr size_t kMaxBlockValue = std::numeric_limits<Block>::max();
    using Container = std::array<Block, kBlockCount>;

    template<bool Set>
    class BitBlockIterator {
    public:

        BitBlockIterator(const BitBlock& block, size_t i) : m_block(block), m_i(i) {
            next();
        }

        void next() {
            if (m_i < N) {
                if constexpr (Set) {
                    m_i += m_block.countr_zero(m_i);
                }
                else {
                    m_i += m_block.countr_one(m_i);
                }
            }
        }

        BitBlockIterator& operator++() {
            m_i++;
            next();
            return *this;
        }

        size_t operator*() const {
            return m_i;
        }

        bool operator==(const BitBlockIterator& rhs) const {
            return m_i == rhs.m_i && &m_block == &rhs.m_block;
        }

    private:
        const BitBlock& m_block;
        size_t m_i;
    };

    template<bool Set>
    class BitBlockView {
    public:

        using Iterator = BitBlockIterator<Set>;

        BitBlockView(const BitBlock& block) : m_block(block) {}

        Iterator begin() const {
            return Iterator(m_block, 0);
        }

        Iterator end() const {
            return Iterator(m_block, N);
        }

    private:
        const BitBlock& m_block;
    };

public:

    BitBlock();

    BitBlock(Container container);

    BitBlock operator~() const;

    BitBlock operator&(const BitBlock& other) const;

    BitBlock operator|(const BitBlock& other) const;

    bool operator==(const BitBlock& other) const;

    bool operator!=(const BitBlock& other) const;

    bool all() const;

    bool none() const;

    bool any() const;

    bool test(size_t index) const;

    std::string to_string() const;

    BitBlock& set(size_t index);

    BitBlock& set();

    BitBlock& reset(size_t index);

    BitBlock& reset();

    size_t countr_zero(size_t startIndex = 0) const;

    size_t countr_one(size_t startIndex = 0) const;

    template<bool Set>
    BitBlockView<Set> bits();

    template<bool Set>
    const BitBlockView<Set> bits() const;

private:
    Container m_container;
};

template<size_t N>
template<bool Set>
const BitBlock<N>::BitBlockView<Set> BitBlock<N>::bits() const {
    return BitBlock::BitBlockView<Set>(*this);
}

template<size_t N>
BitBlock<N>::BitBlock() {
    reset();
}

template<size_t N>
BitBlock<N>::BitBlock(BitBlock::Container container) :
    m_container(container) {

}

template<size_t N>
BitBlock<N> BitBlock<N>::operator~() const {
    Container inverted;
    for (int i = 0; i < m_container.size() - 1; i++) {
        inverted[i] = ~m_container[i];
    }
    inverted.back() = (~m_container.back()) & (kMaxBlockValue >> kLastBlockRemainder);
    return BitBlock<N>(inverted);
}

template<size_t N>
BitBlock<N> BitBlock<N>::operator&(const BitBlock& other) const {
    Container masked;
    for (int i = 0; i < m_container.size(); i++) {
        masked[i] = m_container[i] & other.m_container[i];
    }
    return BitBlock<N>(masked);
}

template<size_t N>
BitBlock<N> BitBlock<N>::operator|(const BitBlock& other) const {
    Container masked;
    for (int i = 0; i < m_container.size(); i++) {
        masked[i] = m_container[i] | other.m_container[i];
    }
    return BitBlock<N>(masked);
}

template<size_t N>
bool BitBlock<N>::operator==(const BitBlock& other) const {
    return m_container == other.m_container;
}

template<size_t N>
bool BitBlock<N>::operator!=(const BitBlock& other) const {
    return m_container != other.m_container;
}

template<size_t N>
bool BitBlock<N>::all() const {
    return std::all_of(m_container.begin(), m_container.end(), [](Block block) {
        return block == kMaxBlockValue;
    });
}

template<size_t N>
bool BitBlock<N>::none() const {
    return std::all_of(m_container.begin(), m_container.end(), [](Block block) {
        return block == 0;
    });
}

template<size_t N>
bool BitBlock<N>::any() const {
    return std::any_of(m_container.begin(), m_container.end(), [](Block block) {
        return block != 0;
    });
}

template<size_t N>
bool BitBlock<N>::test(size_t index) const {
    if constexpr (N > 64) {
        const size_t block = index / 64;
        const size_t indexInBlock = index % 64;
        return m_container[block] & static_cast<Block>(1ull << indexInBlock);
    }
    return m_container[0] & static_cast<Block>(1ull << index);
}

template<size_t N>
std::string BitBlock<N>::to_string() const {
    std::string str;
    str.resize(N);
    for (int i = 0; i < N; i++) {
        str[i] = test(i) ? '1' : '0';
    }
    return str;
}

template<size_t N>
BitBlock<N>& BitBlock<N>::set(size_t index) {
    if constexpr (N > 64) {
        const size_t block = index / 64;
        const size_t indexInBlock = index % 64;
        m_container[block] |= static_cast<Block>(1ull << indexInBlock);
        return *this;
    }
    m_container[0] |= static_cast<Block>(1ull << index);
    return *this;
}

template<size_t N>
BitBlock<N>& BitBlock<N>::set() {
    for (int i = 0; i < kBlockCount; i++) {
        m_container[i] = kMaxBlockValue;
    }
    return *this;
}

template<size_t N>
BitBlock<N>& BitBlock<N>::reset(size_t index) {
    if constexpr (N > 64) {
        const size_t block = index / 64;
        const size_t indexInBlock = index % 64;
        m_container[block] &= static_cast<Block>(~(1ull << indexInBlock));
        return *this;
    }
    m_container[0] &= static_cast<Block>(~(1ull << index));
    return *this;
}

template<size_t N>
BitBlock<N>& BitBlock<N>::reset() {
    for (int i = 0; i < kBlockCount; i++) {
        m_container[i] = 0;
    }
    return *this;
}

template<size_t N>
size_t BitBlock<N>::countr_zero(size_t startIndex) const {
    if constexpr (N > 64) {
        size_t blockIndex = startIndex / 64;
        size_t indexInBlock = startIndex % 64;

        auto block = m_container[blockIndex] >> indexInBlock;

        // overflow to next block
        size_t acc = 0;
        while (block == 0) {

            // on the first iteration, we need to only accumulate the zeros after indexInBlock
            // for the following iterations indexInBlock will be zero, so we accumulate the entire block size
            acc += 64 - indexInBlock;
            indexInBlock = 0;
            blockIndex++;
            if (blockIndex == kBlockCount) {
                return N - startIndex;
            }
            block = m_container[blockIndex];
        }
        return std::countr_zero(block) + acc;
    }
    auto block = m_container[0] >> startIndex;
    if (block == 0) {
        return N - startIndex;
    }
    return std::countr_zero((uint64_t)block);
}

template<size_t N>
size_t BitBlock<N>::countr_one(size_t startIndex) const {
    BitBlock<N> inverted = ~(*this);
    return inverted.countr_zero(startIndex);
}

template<size_t N>
template<bool Set>
BitBlock<N>::BitBlockView<Set> BitBlock<N>::bits() {
    return BitBlockView<Set>(*this);
}

template<bool Set, size_t N, typename Predicate>
void for_each_bit(duk::tools::BitBlock<N> bitBlock, const Predicate& predicate) {
    if constexpr (!Set) {
        bitBlock = ~bitBlock;
    }
    size_t index = 0;
    while (index < N) {
        index += bitBlock.countr_zero(index);
        if (bitBlock.test(index)) {
            predicate(index);
        }
        index++;
    }
}

}

#endif // DUK_TOOLS_BIT_BLOCK_H

