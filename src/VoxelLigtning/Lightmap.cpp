#include "Lightmap.h"

namespace eb {

Lightmap::Lightmap(const glm::i32vec3 &chunk_size)
    : m_chunk_size{chunk_size}
{
    m_map.resize(chunk_size.x * chunk_size.y * chunk_size.y, 0);
}

uint8_t Lightmap::get(const glm::i32vec3 &coords, int32_t channel) const
{
    return (m_map[coords.y * m_chunk_size.y * m_chunk_size.z + coords.z * m_chunk_size.z + coords.x]
            >> (channel << 2))
           & 0xF;
}

uint8_t Lightmap::getR(const glm::i32vec3 &coords) const
{
    return m_map[coords.y * m_chunk_size.y * m_chunk_size.z + coords.z * m_chunk_size.z + coords.x]
           & 0xF;
}

uint8_t Lightmap::getG(const glm::i32vec3 &coords) const
{
    return (m_map[coords.y * m_chunk_size.y * m_chunk_size.z + coords.z * m_chunk_size.z + coords.x]
            >> 4)
           & 0xF;
}

uint8_t Lightmap::getB(const glm::i32vec3 &coords) const
{
    return (m_map[coords.y * m_chunk_size.y * m_chunk_size.z + coords.z * m_chunk_size.z + coords.x]
            >> 8)
           & 0xF;
}

uint8_t Lightmap::getS(const glm::i32vec3 &coords) const
{
    return (m_map[coords.y * m_chunk_size.y * m_chunk_size.z + coords.z * m_chunk_size.z + coords.x]
            >> 12)
           & 0xF;
}

void Lightmap::setR(const glm::i32vec3 &coords, int32_t value)
{
    const int32_t index = coords.y * m_chunk_size.y * m_chunk_size.z + coords.z * m_chunk_size.z
                          + coords.x;
    m_map[index] = (m_map[index] & 0xFFF0) | value;
}

void Lightmap::setG(const glm::i32vec3 &coords, int32_t value)
{
    const int32_t index = coords.y * m_chunk_size.y * m_chunk_size.z + coords.z * m_chunk_size.z
                          + coords.x;
    m_map[index] = (m_map[index] & 0xFF0F) | (value << 4);
}

void Lightmap::setB(const glm::i32vec3 &coords, int32_t value)
{
    const int32_t index = coords.y * m_chunk_size.y * m_chunk_size.z + coords.z * m_chunk_size.z
                          + coords.x;
    m_map[index] = (m_map[index] & 0xF0FF) | (value << 8);
}

void Lightmap::setS(const glm::i32vec3 &coords, int32_t value)
{
    const int32_t index = coords.y * m_chunk_size.y * m_chunk_size.z + coords.z * m_chunk_size.z
                          + coords.x;
    m_map[index] = (m_map[index] & 0x0FFF) | (value << 12);
}

void Lightmap::set(const glm::i32vec3 &coords, int32_t channel, int32_t value)
{
    const int32_t index = coords.y * m_chunk_size.y * m_chunk_size.z + coords.z * m_chunk_size.z
                          + coords.x;
    m_map[index] = (m_map[index] & (0xFFFF & (~(0xF << (channel * 4)))))
                   | (value << (channel << 2));
}

} // namespace eb
