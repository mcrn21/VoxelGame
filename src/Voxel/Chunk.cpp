#include "Chunk.h"
#include "Chunks.h"

#include <glm/gtc/noise.hpp>
#include <spdlog/spdlog.h>

namespace eb {

Chunk::Chunk(const glm::i32vec3 &position, Chunks *chunks)
    : m_chunks{chunks}
    , m_position{position}
    , m_light_map{chunks->getChunkSize()}
    , m_modified{false}
{
    auto &chunk_size = chunks->getChunkSize();
    m_voxels.resize(chunk_size.x * chunk_size.y * chunk_size.z, Voxel{0});
}

Chunks *Chunk::getChunks() const
{
    return m_chunks;
}

const glm::i32vec3 &Chunk::getPosition() const
{
    return m_position;
}

const Voxel *Chunk::getVoxel(const glm::i32vec3 &voxel_coords) const
{
    int32_t index = voxelCoordsToIndex(voxel_coords);
    return (index < 0 || index >= m_voxels.size()) ? nullptr : &m_voxels[index];
}

void Chunk::setVoxel(const glm::i32vec3 &voxel_coords, const Voxel &voxel)
{
    m_voxels[voxelCoordsToIndex(voxel_coords)] = voxel;
    m_modified = true;
    m_chunks->m_chunks_modfied = true;
}

Lightmap &Chunk::getLightmap()
{
    return m_light_map;
}

int32_t Chunk::voxelCoordsToIndex(const glm::i32vec3 &voxel_coords) const
{
    auto &chunk_size = m_chunks->getChunkSize();
    return (voxel_coords.y * chunk_size.z + voxel_coords.z) * chunk_size.x + voxel_coords.x;
}

} // namespace eb
