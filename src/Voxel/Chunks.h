#ifndef EB_VOXEL_CHUNKS_H
#define EB_VOXEL_CHUNKS_H

#include "../EngineObject.h"
#include "../Graphics/ChunkMesh.h"
#include "../Graphics/Texture.h"
#include "Chunk.h"

#include <memory>

namespace eb {

class LightSolver;

class Chunks : public EngineObject
{
    friend class LightSolver;
    friend class Chunk;

public:
    Chunks(const glm::i32vec3 &chunks_size,
           const glm::i32vec3 &chunk_size,
           float voxel_size,
           float texture_size,
           const std::shared_ptr<Texture> &atlas_texture,
           Engine *engine);
    ~Chunks() = default;

    const glm::i32vec3 &getChunksSize() const;
    const glm::i32vec3 &getChunkSize() const;
    float getVoxelSize() const;
    float getTextureSize() const;
    std::shared_ptr<Texture> getAtlasTexture() const;

    Chunk *getChunk(const glm::i32vec3 &chunk_coords);
    Chunk *getChunkByVoxel(const glm::i32vec3 &voxel_coords);
    Chunk *getChunkByGlobal(const glm::vec3 &global_coords);

    const Voxel *getVoxel(const glm::i32vec3 &voxel_coords) const;
    const Voxel *getVoxelByGlobal(const glm::vec3 &global_coords) const;

    void setVoxel(const glm::i32vec3 &voxel_coords, const Voxel &voxel);
    void setVoxelByGlobal(const glm::vec3 &global_coords, const Voxel &voxel);

    uint8_t getLight(const glm::i32vec3 &voxel_coords, int32_t channel) const;

    bool isVoxelBlocked(const glm::i32vec3 &voxel_coords) const;

    bool containsChunk(const glm::i32vec3 &chunk_coords) const;
    bool containsVoxel(const glm::i32vec3 &voxel_coords) const;

    const Voxel *rayCast(glm::vec3 start,
                         glm::vec3 direction,
                         float max_dist,
                         glm::vec3 &end,
                         glm::vec3 &norm,
                         glm::vec3 &iend);

    void forEachVoxels(
        const std::function<void(const glm::i32vec3 &, const glm::i32vec3 &)> &func) const;

    void forEachVoxelsInChunk(
        const glm::i32vec3 &chunk_coords,
        const std::function<void(const glm::i32vec3 &, const glm::i32vec3 &)> &func) const;

    void update();

    // Render chunks
    void draw(const RenderTarget &render_target) const;

private:
    int32_t chunkCoordsToIndex(const glm::i32vec3 &chunk_coords) const;

    void setChunkData(const glm::i32vec3 &chunk_coords);

private:
    struct ChunkState
    {
        ChunkState() {}
        ChunkState(const glm::i32vec3 &position,
                   Chunks *chunks,
                   const std::shared_ptr<Texture> &texture,
                   Engine *engine)
            : chunk{std::make_unique<Chunk>(position, chunks)}
            , mesh{std::make_unique<ChunkMesh>(engine, texture)}
        {}

        ChunkState(ChunkState &&other)
            : chunk{std::move(other.chunk)}
            , mesh{std::move(other.mesh)}
        {}

        ChunkState &operator=(ChunkState &&other)
        {
            chunk = std::move(other.chunk);
            mesh = std::move(other.mesh);
            return *this;
        }

        std::unique_ptr<Chunk> chunk;
        std::unique_ptr<ChunkMesh> mesh;
    };

    glm::i32vec3 m_chunks_size;
    glm::i32vec3 m_chunk_size;
    float m_voxel_size;
    float m_texture_size;
    std::shared_ptr<Texture> m_atlas_texture;

    std::vector<std::unique_ptr<ChunkState>> m_chunk_states;
    bool m_chunks_modfied;
};

} // namespace eb

#endif // EB_VOXEL_CHUNKS_H
