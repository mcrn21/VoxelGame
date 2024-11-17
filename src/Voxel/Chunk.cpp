#include "Chunk.h"

#include <glm/gtc/noise.hpp>
#include <spdlog/spdlog.h>

namespace eb {

Chunk::Chunk(const glm::i32vec3 &position,
             const glm::i32vec3 &chunk_size,
             float voxel_size,
             float texture_size)
    : m_position{position}
    , m_chunk_size{chunk_size}
    , m_voxel_size{voxel_size}
    , m_texture_size{texture_size}
{
    m_voxels.resize(m_chunk_size.x * m_chunk_size.y * m_chunk_size.z, Voxel{0});

    forEach([this](const Voxel *voxel, const glm::i32vec3 &index) {
        int32_t real_x = index.x + m_position.x * m_chunk_size.x * m_voxel_size;
        int32_t real_z = index.z * m_voxel_size + m_position.z * m_chunk_size.z * m_voxel_size;
        int32_t real_y = index.y * m_voxel_size + m_position.y * m_chunk_size.y * m_voxel_size;

        int id = glm::perlin(glm::vec3(real_x * 0.125f, real_y * 0.125f, real_z * 0.125f)) > 0.1f;
        if (real_y <= 2)
            id = 2;

        setVoxel(index, id);
    });
}

const glm::i32vec3 &Chunk::getPosition() const
{
    return m_position;
}

const glm::i32vec3 &Chunk::getChunkSize() const
{
    return m_chunk_size;
}

float Chunk::getVoxelSize() const
{
    return m_voxel_size;
}

float Chunk::getTextureSize() const
{
    return m_texture_size;
}

const Voxel *Chunk::getVoxel(const glm::i32vec3 &index) const
{
    if (!contains(index))
        return nullptr;
    return &m_voxels[(index.y * m_chunk_size.z + index.z) * m_chunk_size.x + index.x];
}

void Chunk::setVoxel(const glm::i32vec3 &index, int32_t id)
{
    if (!contains(index))
        return;
    m_voxels[(index.y * m_chunk_size.z + index.z) * m_chunk_size.x + index.x].id = id;
}

bool Chunk::isBlocked(const glm::i32vec3 &index) const
{
    auto *voxel = getVoxel(index);
    return voxel ? voxel->id != 0 : false;
}

bool Chunk::contains(const glm::i32vec3 &index) const
{
    return index.x >= 0 && index.x < m_chunk_size.x && index.y >= 0 && index.y < m_chunk_size.y
           && index.z >= 0 && index.z < m_chunk_size.z;
}

void Chunk::forEach(const std::function<void(const Voxel *, const glm::i32vec3 &)> &func) const
{
    if (!func)
        return;

    glm::i32vec3 index{0};
    for (index.y = 0; index.y < m_chunk_size.y; ++index.y) {
        for (index.z = 0; index.z < m_chunk_size.z; ++index.z) {
            for (index.x = 0; index.x < m_chunk_size.x; ++index.x) {
                func(getVoxel(index), index);
            }
        }
    }
}

} // namespace eb
