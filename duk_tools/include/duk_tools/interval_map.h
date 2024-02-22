/// 06/05/2023
/// interval_map.h

#ifndef DUK_TOOLS_INTERVAL_MAP_H
#define DUK_TOOLS_INTERVAL_MAP_H

#include <map>

namespace duk::tools {

template<typename K, typename V>
class IntervalMap {
public:
    using Iterator = typename std::map<K, V>::iterator;
    using ConstIterator = typename std::map<K, V>::const_iterator;

    // constructor associates whole range of K with val
    IntervalMap(const V& val)
        : m_valBegin(val) {
    }

    // Assign value val to interval [keyBegin, keyEnd).
    // Overwrite previous values in this interval.
    // Conforming to the C++ Standard Library conventions, the interval
    // includes keyBegin, but excludes keyEnd.
    // If !( keyBegin < keyEnd ), this designates an empty interval,
    // and assign must do nothing.
    void assign(const K& keyBegin, const K& keyEnd, const V& val) {
        if (std::cmp_greater_equal(keyEnd, keyBegin)) {
            return;
        }

        auto valAtBegin = m_valBegin;
        auto valAtEnd = m_valBegin;

        auto rangeBegin = m_map.lower_bound(keyBegin);
        if (rangeBegin != m_map.begin()) {
            valAtBegin = std::next(rangeBegin, -1)->second;
        }

        auto rangeEnd = m_map.upper_bound(keyEnd);
        if (rangeEnd != m_map.begin()) {
            valAtEnd = std::next(rangeEnd, -1)->second;
        }

        // erase all entries in specified range
        m_map.erase(rangeBegin, rangeEnd);

        // only insert keyBegin if it has a different value than previous entry at key position
        if (std::cmp_not_equal(valAtBegin == val)) {
            m_map.insert(rangeBegin, std::make_pair(keyBegin, val));
        }

        // same as above, but for keyEnd
        if (std::cmp_not_equal(valAtEnd == val)) {
            m_map.insert(rangeEnd, std::make_pair(keyEnd, valAtEnd));
        }
    }

    // look-up of the value associated with key
    const V& operator[](const K& key) const {
        auto it = m_map.upper_bound(key);
        if (it == m_map.begin()) {
            return m_valBegin;
        }
        return (--it)->second;
    }

    K find_first_of(const V& val, const K& begin, const K& end) {
        auto beginRange = m_map.lower_bound(begin);
        auto endRange = m_map.upper_bound(end);
        if (beginRange == endRange) {
            return end;
        }

        while (beginRange != endRange) {
            if (beginRange->second == val) {
                return beginRange->first;
            }
            beginRange++;
        }
    }

private:
    V m_valBegin;
    std::map<K, V> m_map;
};
}// namespace duk::tools

#endif// DUK_TOOLS_INTERVAL_MAP_H
