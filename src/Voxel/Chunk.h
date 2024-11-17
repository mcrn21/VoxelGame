#ifndef EB_VOXEL_CHUNK_H
#define EB_VOXEL_CHUNK_H

#include "Voxel.h"

#include <glm/glm.hpp>

#include <functional>
#include <vector>

namespace eb {

class Chunk
{
public:
    Chunk(const glm::i32vec3 &position,
          const glm::i32vec3 &chunk_size,
          float voxel_size,
          float texture_size);
    ~Chunk() = default;

    const glm::i32vec3 &getPosition() const;
    const glm::i32vec3 &getChunkSize() const;
    float getVoxelSize() const;
    float getTextureSize() const;

    const Voxel *getVoxel(const glm::i32vec3 &index) const;
    void setVoxel(const glm::i32vec3 &index, int32_t id);

    bool isBlocked(const glm::i32vec3 &index) const;
    bool contains(const glm::i32vec3 &index) const;

    void forEach(const std::function<void(const Voxel *, const glm::i32vec3 &)> &func) const;

private:
    glm::i32vec3 m_position;
    glm::i32vec3 m_chunk_size;
    float m_voxel_size;
    float m_texture_size;
    std::vector<Voxel> m_voxels;
};

} // namespace eb

#endif // EB_VOXEL_CHUNK_H
