#ifndef EB_UTILS_SPARSESET_H
#define EB_UTILS_SPARSESET_H

#include <assert.h>
#include <stdint.h>
#include <type_traits>
#include <vector>

namespace eb {

template<typename T, typename U>
class SparseSet
{
public:
    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;

public:
    SparseSet() {}
    ~SparseSet() = default;

    template<typename... Ts>
    U &insert(const T &index, Ts &&...constructor_args)
    {
        if (index < m_sparse.size()) {
            if (m_sparse[index] < m_dense.size() && m_dense[m_sparse[index]] == index) {
                U &value{m_dense_v[m_sparse[index]]};
                value.~U();
                if constexpr (std::is_aggregate_v<U>)
                    new (&value) T{std::forward<Ts>(constructor_args)...};
                else
                    new (&value) T(std::forward<Ts>(constructor_args)...);
                return value;
            }

            m_sparse[index] = static_cast<uint32_t>(m_dense.size());
        } else {
            m_sparse.resize(static_cast<uint32_t>(index) + 1, static_cast<uint32_t>(m_dense.size()));
        }

        m_dense.push_back(index);
        if constexpr (std::is_aggregate_v<U>)
            return m_dense_v.emplace_back(std::move(U{std::forward<Ts>(constructor_args)...}));
        else
            return m_dense_v.emplace_back(std::forward<Ts>(constructor_args)...);
    }

    bool has(const T &index) const
    {
        if (index >= m_sparse.size())
            return false;

        auto s = m_sparse[index];
        if (s >= m_dense.size())
            return false;

        return index == m_dense[s];
    }

    U &get(const T &index)
    {
        assert(has(index));
        assert(m_sparse[index] < m_dense_v.size());
        return m_dense_v[m_sparse[index]];
    }

    bool remove(const T &index)
    {
        if (has(index)) {
            const T last{m_dense.back()};

            std::swap(m_dense.back(), m_dense[m_sparse[index]]);
            std::swap(m_dense_v.back(), m_dense_v[m_sparse[index]]);

            assert(last < m_sparse.size());

            std::swap(m_sparse[last], m_sparse[index]);

            m_dense.pop_back();
            m_dense_v.pop_back();

            return true;
        }
        return false;
    }

    int32_t getSize() const { return m_dense.size(); }
    const std::vector<U> &getData() const { return m_dense_v; }
    int32_t getIndex(const T &index) const
    {
        assert(has(index));
        return m_sparse[index];
    }

private:
    std::vector<T> m_dense;
    std::vector<U> m_dense_v;
    std::vector<uint32_t> m_sparse;
};

} // namespace eb

#endif // EB_UTILS_SPARSESET_H
