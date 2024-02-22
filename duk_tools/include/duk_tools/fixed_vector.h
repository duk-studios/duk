/// 06/05/2023
/// fixed_vector.h

#ifndef DUK_TOOLS_FIXED_VECTOR_H
#define DUK_TOOLS_FIXED_VECTOR_H

#include <duk_macros/macros.h>

#include <array>
#include <memory>
#include <stdexcept>

namespace duk::tools {

template<typename T, size_t N>
class FixedVector {
public:
    static_assert(N > 0, "N has to be greater than 0 in FixedVector");

    using Iterator = typename std::array<T, N>::iterator;
    using ConstIterator = typename std::array<T, N>::const_iterator;

    FixedVector() :
        m_size(0) {

    }

    explicit FixedVector(size_t initialSize) :
        m_size(0) {
        resize(initialSize);
    }

    FixedVector(size_t initialSize, const T& initialValue) :
        m_size(0) {
        for (int i = 0; i < initialSize; i++) {
            push_back(initialValue);
        }
    }

    FixedVector(const FixedVector& other) {
        *this = other;
    }

    FixedVector(FixedVector&& other) {
        *this = std::move(other);
    }

    FixedVector& operator=(const FixedVector& other) noexcept {
        m_size = other.m_size;
        memcpy(m_array.data(), other.m_array.data(), m_size * sizeof(T));
    }

    FixedVector& operator=(FixedVector&& other) noexcept {
        for (auto& value : other){
            push_back(std::move(value));
        }
        other.clear();
    }

    ~FixedVector() {
        clear();
    }

    void push_back(const T& value) {
        if (m_size >= N){
            throw std::out_of_range("push_back on a full FixedVector");
        }
        m_array[m_size++] = value;
    }

    void push_back(T&& value) {
        if (m_size >= N){
            throw std::out_of_range("push_back on a full FixedVector");
        }
        m_array[m_size++] = std::move(value);
    }

    template<typename ...Args>
    T& emplace_back(Args&&... args) requires std::is_trivially_constructible_v<T> {
        if (m_size >= N){
            throw std::out_of_range("emplace_back on a full FixedVector");
        }
        return at(m_size++) = T(std::forward<Args>(args)...);
    }

    template<typename ...Args>
    T& emplace_back(Args&&... args) requires (std::is_trivially_constructible_v<T> == false) {
        if (m_size >= N){
            throw std::out_of_range("emplace_back on a full FixedVector");
        }
        const auto index = m_size++;
        construct(m_array[index], std::forward<Args>(args)...);
        return at(index);
    }

    void pop_back() requires std::is_trivially_destructible_v<T> {
        auto newSize = m_size - 1;
        m_size = newSize;
    }

    void pop_back() requires (std::is_trivially_destructible_v<T> == false) {
        auto newSize = m_size - 1;
        destroy(at(newSize));
        m_size = newSize;
    }

    void clear() {
        // call destructor for every element in vector
        resize(0);
    }

    void resize(size_t newSize) {
        if (newSize > N){
            throw std::out_of_range("expand with newSize > N on FixedVector");
        }

        if (newSize > m_size) {
            append(newSize - m_size);
        }
        else if (newSize < m_size) {
            erase_at_end(m_size - newSize);
        }
    }

    T& at(size_t index) {
        if (index >= m_size) {
            throw std::out_of_range("index out of range on FixedVector");
        }
        return m_array[index];
    }

    const T& at(size_t index) const {
        if (index >= m_size) {
            throw std::out_of_range("index out of range on FixedVector");
        }
        return m_array[index];
    }

    T& operator[](size_t index) {
        assert(index < m_size);
        return m_array[index];
    }

    const T& operator[](size_t index) const {
        assert(index < m_size);
        return m_array[index];
    }

    T& back() {
        return at(m_size - 1);
    }

    const T& back() const {
        return at(m_size - 1);
    }

    T& front() {
        return at(0);
    }

    const T& front() const {
        return at(0);
    }

    DUK_NO_DISCARD Iterator begin() {
        return m_array.begin();
    }

    DUK_NO_DISCARD Iterator end() {
        return m_array.begin() + m_size;
    }

    DUK_NO_DISCARD ConstIterator begin() const {
        return m_array.begin();
    }

    DUK_NO_DISCARD ConstIterator end() const {
        return m_array.begin() + m_size;
    }

    DUK_NO_DISCARD T* data() {
        return m_array.data();
    }

    DUK_NO_DISCARD const T* data() const {
        return m_array.data();
    }

    DUK_NO_DISCARD size_t size() const {
        return m_size;
    }

    DUK_NO_DISCARD size_t max_size() const {
        return N;
    }

    DUK_NO_DISCARD bool empty() const {
        return m_size == 0;
    }

private:

    void destroy(T& value) const {
        value.~T();
    }

    template<typename ...Args>
    void construct(T& value, Args&&... args) const {
        ::new(&value) T(std::forward<Args>(args)...);
    }

    void append(size_t n) requires std::is_trivially_constructible_v<T> {
        m_size += n;
    }

    void append(size_t n) requires (std::is_trivially_constructible_v<T> == false) {
        //construct new elements
        auto newSize = m_size + n;
        for (int i = m_size; i < newSize; i++) {
            construct(m_array[i]);
        }
        m_size = newSize;
    }

    void erase_at_end(size_t n) requires std::is_trivially_destructible_v<T> {
        assert(n <= m_size);
        m_size -= n;
    }

    void erase_at_end(size_t n) requires (std::is_trivially_destructible_v<T> == false) {
        assert(n <= m_size);
        //destroy excess elements
        auto newSize = m_size - n;
        for (auto i = newSize; i < m_size; i++) {
            destroy(m_array[i]);
        }
        m_size = newSize;
    }

private:
    std::array<T, N> m_array;
    size_t m_size;
};

}

#endif // DUK_TOOLS_FIXED_VECTOR_H
