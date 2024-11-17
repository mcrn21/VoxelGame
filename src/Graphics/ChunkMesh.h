#ifndef EB_GRAPHICS_CHUNKMESH_H
#define EB_GRAPHICS_CHUNKMESH_H

#include "../EngineObject.h"
#include "Drawable.h"
#include "Texture.h"
#include "Transformable.h"
#include "VertexBuffer.h"
#include "VoxelVertex.h"

#include <memory>

namespace eb {

class Chunk;
class Chunks;

class ChunkMesh : public EngineObject, public Drawable, public Transformable
{
public:
    struct AmbientOcclusion
    {
        void clear()
        {
            a = b = c = d = e = f = g = h = 0.0f;
            p1 = p2 = p3 = p4 = p5 = p6 = 1.0f;
        }

        float light = 1.0f;
        float factor = 0.15f;
        float a, b, c, d, e, f, g, h = 0.0f;
        float p1, p2, p3, p4, p5, p6 = 1.0f;
    };

    ChunkMesh(Engine *engine);
    ChunkMesh(Engine *engine, const std::shared_ptr<Texture> &texture);
    ~ChunkMesh() = default;

    std::shared_ptr<Texture> getTexture() const;
    void setTexture(const std::shared_ptr<Texture> &texture);

    void create(Chunk *chunk);
    void create(Chunks *chunks, const glm::i32vec3 &index);

    void draw(const RenderTarget &render_target) const;

private:
    void createUpSide(std::vector<VoxelVertex> &vertices,
                      const glm::vec3 &offset,
                      const glm::vec4 &uv,
                      float voxel_size,
                      const AmbientOcclusion &ao);
    void createDownSide(std::vector<VoxelVertex> &vertices,
                        const glm::vec3 &offset,
                        const glm::vec4 &uv,
                        float voxel_size,
                        const AmbientOcclusion &ao);
    void createLeftSide(std::vector<VoxelVertex> &vertices,
                        const glm::vec3 &offset,
                        const glm::vec4 &uv,
                        float voxel_size,
                        const AmbientOcclusion &ao);
    void createRightSide(std::vector<VoxelVertex> &vertices,
                         const glm::vec3 &offset,
                         const glm::vec4 &uv,
                         float voxel_size,
                         const AmbientOcclusion &ao);
    void createFrontSide(std::vector<VoxelVertex> &vertices,
                         const glm::vec3 &offset,
                         const glm::vec4 &uv,
                         float voxel_size,
                         const AmbientOcclusion &ao);
    void createBackSide(std::vector<VoxelVertex> &vertices,
                        const glm::vec3 &offset,
                        const glm::vec4 &uv,
                        float voxel_size,
                        const AmbientOcclusion &ao);

private:
    std::shared_ptr<Texture> m_texture;
    VertexBuffer<VoxelVertex, 4, 2, 1> m_vertex_buffer;
};

} // namespace eb

#endif // EB_GRAPHICS_CHUNKMESH_H
