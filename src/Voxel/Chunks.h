#ifndef EB_VOXEL_CHUNKS_H
#define EB_VOXEL_CHUNKS_H

#include "../EngineObject.h"
#include "../Graphics/ChunkMesh.h"
#include "../Graphics/Texture.h"
#include "Chunk.h"

#include <memory>

namespace eb {

class Chunks : public EngineObject
{
public:
    Chunks(const glm::i32vec3 &chunks_size,
           const glm::i32vec3 &chunk_size,
           float voxel_size,
           float texture_size,
           Engine *engine);
    ~Chunks() = default;

    const glm::i32vec3 &getChunksSize() const;
    const glm::i32vec3 &getChunkSize() const;
    float getVoxelSize() const;
    float getTextureSize() const;

    const Chunk *getChunk(const glm::i32vec3 &index) const;
    const Chunk *getChunkByGlobal(const glm::vec3 &coords) const;

    const Voxel *getVoxel(const glm::i32vec3 &index) const;
    const Voxel *getVoxelByGlobal(const glm::vec3 &coords) const;
    void setVoxel(const glm::i32vec3 &index, int32_t id);
    void setVoxelByGlobal(const glm::vec3 &coords, int32_t id);

    bool isBlocked(const glm::i32vec3 &index) const;

    bool containsChunk(const glm::i32vec3 &index) const;
    bool containsVoxel(const glm::i32vec3 &index) const;

    glm::i32vec3 getChunkCoordsByGlobal(const glm::vec3 &coords) const;
    int32_t getChunkIndexByGlobal(const glm::vec3 &coords) const;

    const Voxel *rayCast(glm::vec3 start,
                         glm::vec3 direction,
                         float max_dist,
                         glm::vec3 &end,
                         glm::vec3 &norm,
                         glm::vec3 &iend);

    void forEach(const std::function<void(const Chunk *, const glm::i32vec3 &)> &func) const;

private:
    glm::i32vec3 m_chunks_size;
    glm::i32vec3 m_chunk_size;
    float m_voxel_size;
    float m_texture_size;
    std::vector<std::unique_ptr<Chunk>> m_chunks;
    std::vector<std::unique_ptr<ChunkMesh>> m_meshes;
};

} // namespace eb

#endif // EB_VOXEL_CHUNKS_H
