#ifndef EB_GRAPHICS_CHUNKMESH_H
#define EB_GRAPHICS_CHUNKMESH_H

#include "../EngineObject.h"
#include "Drawable.h"
#include "Texture.h"
#include "Transformable.h"
#include "VertexBuffer.h"

#include <memory>

namespace eb {

class Chunk;
class Chunks;

struct VoxelVertex
{
    VoxelVertex()
        : position{0.0f}
        , tex_coords{0.0f}
        , light{1.0f}
    {}
    VoxelVertex(const glm::vec3 &position, const glm::vec2 &tex_coords, const glm::vec4 &light)
        : position{position}
        , tex_coords{tex_coords}
        , light{light}
    {}
    ~VoxelVertex() = default;

    glm::vec3 position;
    glm::vec2 tex_coords;
    glm::vec4 light;
};

class ChunkMesh : public EngineObject, public Drawable, public Transformable
{
public:
    struct Light
    {
        Light()
            : p1{0.0f, 0.0f, 0.0f, 1.0f}
            , p2{0.0f, 0.0f, 1.0f, 1.0f}
            , p3{0.0f, 0.0f, 1.0f, 1.0f}
            , p4{0.0f, 0.0f, 1.0f, 1.0f}
        {
            f0 = 3.0f;
            f1 = f0 * 35.0f;
        }
        void fill(const glm::vec4 &light) { p1 = p2 = p3 = p4 = light; }
        void clear() { p1 = p2 = p3 = p4 = {0.0f, 0.0f, 0.0f, 1.0f}; }

        void calculate(Chunks *chunks,
                       const glm::i32vec3 &voxel_coords,
                       const glm::i32vec3 (&neighbours)[9]);

        glm::vec4 p1;
        glm::vec4 p2;
        glm::vec4 p3;
        glm::vec4 p4;

        float f0;
        float f1;
    };

    ChunkMesh(Engine *engine);
    ChunkMesh(Engine *engine, const std::shared_ptr<Texture> &texture);
    ~ChunkMesh() = default;

    std::shared_ptr<Texture> getTexture() const;
    void setTexture(const std::shared_ptr<Texture> &texture);

    void create(Chunks *chunks, const glm::i32vec3 &chunk_coords);

    void draw(const RenderTarget &render_target, const RenderState &render_state) const;

private:
    void createUpSide(std::vector<VoxelVertex> &vertices,
                      std::vector<uint32_t> &indices,
                      const glm::vec3 &offset,
                      const glm::vec4 &uv,
                      float voxel_size,
                      const Light &light);
    void createDownSide(std::vector<VoxelVertex> &vertices,
                        std::vector<uint32_t> &indices,
                        const glm::vec3 &offset,
                        const glm::vec4 &uv,
                        float voxel_size,
                        const Light &light);
    void createLeftSide(std::vector<VoxelVertex> &vertices,
                        std::vector<uint32_t> &indices,
                        const glm::vec3 &offset,
                        const glm::vec4 &uv,
                        float voxel_size,
                        const Light &light);
    void createRightSide(std::vector<VoxelVertex> &vertices,
                         std::vector<uint32_t> &indices,
                         const glm::vec3 &offset,
                         const glm::vec4 &uv,
                         float voxel_size,
                         const Light &light);
    void createFrontSide(std::vector<VoxelVertex> &vertices,
                         std::vector<uint32_t> &indices,
                         const glm::vec3 &offset,
                         const glm::vec4 &uv,
                         float voxel_size,
                         const Light &light);
    void createBackSide(std::vector<VoxelVertex> &vertices,
                        std::vector<uint32_t> &indices,
                        const glm::vec3 &offset,
                        const glm::vec4 &uv,
                        float voxel_size,
                        const Light &light);

    void appendIndices(std::vector<uint32_t> &indices, int32_t vertices_offset) const;

private:
    std::shared_ptr<Texture> m_texture;
    VertexBuffer<VoxelVertex, 2, 4> m_vertex_buffer;
};

} // namespace eb

#endif // EB_GRAPHICS_CHUNKMESH_H
