#ifndef EB_VOXELLIGHTNING_LIGHTMAP_H
#define EB_VOXELLIGHTNING_LIGHTMAP_H

#include <stdint.h>
#include <vector>

#include <glm/glm.hpp>

namespace eb {

class Lightmap
{
public:
    Lightmap(const glm::i32vec3 &chunk_size);
    ~Lightmap() = default;

    uint8_t get(const glm::i32vec3 &coords, int32_t channel) const;
    uint8_t getR(const glm::i32vec3 &coords) const;
    uint8_t getG(const glm::i32vec3 &coords) const;
    uint8_t getB(const glm::i32vec3 &coords) const;
    uint8_t getS(const glm::i32vec3 &coords) const;

    void setR(const glm::i32vec3 &coords, int32_t value);
    void setG(const glm::i32vec3 &coords, int32_t value);
    void setB(const glm::i32vec3 &coords, int32_t value);
    void setS(const glm::i32vec3 &coords, int32_t value);
    void set(const glm::i32vec3 &coords, int32_t channel, int32_t value);

private:
    glm::i32vec3 m_chunk_size;
    std::vector<uint16_t> m_map;
};

} // namespace eb

#endif // EB_VOXELLIGHTNING_LIGHTMAP_H
