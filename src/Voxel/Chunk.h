#ifndef EB_VOXEL_CHUNK_H
#define EB_VOXEL_CHUNK_H

#include "../VoxelLigtning/Lightmap.h"
#include "Voxel.h"

#include <glm/glm.hpp>

#include <vector>

namespace eb {

class Chunks;
class LightSolver;

class Chunk
{
    friend class Chunks;
    friend class LightSolver;

public:
    Chunk(const glm::i32vec3 &position, Chunks *chunks);
    ~Chunk() = default;

    Chunks *getChunks() const;
    const glm::i32vec3 &getPosition() const;

    const Voxel *getVoxel(const glm::i32vec3 &voxel_coords) const;
    void setVoxel(const glm::i32vec3 &voxel_coords, const Voxel &voxel);

    Lightmap &getLightmap();

    int32_t voxelCoordsToIndex(const glm::i32vec3 &voxel_coords) const;

private:
    Chunks *m_chunks;
    glm::i32vec3 m_position;
    std::vector<Voxel> m_voxels;
    Lightmap m_light_map;
    bool m_modified;
};

} // namespace eb

#endif // EB_VOXEL_CHUNK_H
