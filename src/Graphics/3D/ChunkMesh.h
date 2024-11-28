#ifndef EB_GRAPHICS_3D_CHUNKMESH_H
#define EB_GRAPHICS_3D_CHUNKMESH_H

#include "../../EngineObject.h"
#include "../3D/Material.h"
#include "../Common/Drawable.h"
#include "../Common/Texture.h"
#include "../Common/VertexArray.h"

#include <memory>

namespace eb {

class Chunk;
class Chunks;

struct VoxelVertex
{
    glm::vec3 position{0.0f};
    glm::vec3 normal{0.0f};
    glm::vec2 tex_coords{0.0f};
    glm::vec4 light{1.0f};
};

class ChunkMesh : public EngineObject, public Drawable
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

    const Material &getMaterial() const;
    void setMaterial(const Material &material);

    void create(Chunks *chunks, const glm::i32vec3 &chunk_coords);

    void draw(const RenderState &render_state) const;
    // void draw(const RenderTarget &render_target, const RenderState3D &render_state) const;

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
    Material m_material;
    VertexArray m_vertex_array;
};

} // namespace eb

#endif // EB_GRAPHICS_3D_CHUNKMESH_H
