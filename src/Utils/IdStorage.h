#ifndef EB_UTILS_IDSTORAGE_H
#define EB_UTILS_IDSTORAGE_H

#include <limits>
#include <set>
#include <stdint.h>
#include <vector>

namespace eb {

class Id
{
public:
    static Id invalid;

    Id()
        : m_id(std::numeric_limits<uint64_t>::max())
    {}
    explicit Id(uint64_t id)
        : m_id(id)
    {}
    Id(uint32_t index, uint32_t version)
        : m_id(uint64_t(index) | uint64_t(version) << 32UL)
    {}
    Id(const Id &other)
        : m_id(other.m_id)
    {}

    bool operator==(const Id &other) const { return m_id == other.m_id; }
    bool operator!=(const Id &other) const { return m_id != other.m_id; }

    Id operator=(const Id &other)
    {
        m_id = other.m_id;
        return *this;
    }

    template<typename T>
    bool operator<(const T &right) const
    {
        return static_cast<uint32_t>(*this) < static_cast<uint32_t>(right);
    }
    template<typename T>
    bool operator<=(const T &right) const
    {
        return static_cast<uint32_t>(*this) <= static_cast<uint32_t>(right);
    }
    template<typename T>
    bool operator>(const T &right) const
    {
        return static_cast<uint32_t>(*this) > static_cast<uint32_t>(right);
    }
    template<typename T>
    bool operator>=(const T &right) const
    {
        return static_cast<uint32_t>(*this) >= static_cast<uint32_t>(right);
    }

    operator int32_t() const { return static_cast<int32_t>(m_id & 0xffffffffUL); }
    operator uint32_t() const { return static_cast<uint32_t>(m_id & 0xffffffffUL); }
    operator uint64_t() const { return static_cast<uint64_t>(m_id & 0xffffffffUL); }
    operator bool() const { return m_id != 0 && m_id < std::numeric_limits<uint64_t>::max(); }

    uint64_t getId() const { return m_id; }
    uint32_t getIndex() const { return m_id & 0xffffffffUL; }
    uint32_t getVersion() const { return m_id >> 32; }

private:
    uint64_t m_id;
};

class IdStorage
{
public:
    IdStorage();
    ~IdStorage() = default;

    Id create();
    void destroy(const Id &id);
    Id get(uint32_t index) const;
    bool isValid(const Id &id) const;
    void resize(int32_t size);
    void clear();
    int32_t size() const;

private:
    uint32_t m_next_index;
    std::set<uint32_t> m_free_indexes;
    std::vector<uint32_t> m_versions;
};

} // namespace eb

#endif // EB_UTILS_IDSTORAGE_H
