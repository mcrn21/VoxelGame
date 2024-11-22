#ifndef EB_GRAPHICS_MESH_H
#define EB_GRAPHICS_MESH_H

#include "Drawable.h"
#include "Texture.h"
#include "Transformable.h"
#include "VertexBuffer.h"

#include <memory>
#include <vector>

namespace eb {

struct MeshVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

class Mesh : public Drawable, public Transformable
{
public:
    Mesh();
    Mesh(const std::vector<MeshVertex> &vertices,
         const std::vector<uint32_t> &indices,
         const std::vector<std::shared_ptr<Texture>> &textures);
    ~Mesh() = default;

    void create(const std::vector<MeshVertex> &vertices,
                const std::vector<uint32_t> &indices,
                const std::vector<std::shared_ptr<Texture>> &textures);
    void destroy();

    void draw(const RenderTarget &render_target, const RenderState &render_state) const;

private:
    std::vector<MeshVertex> m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<std::shared_ptr<Texture>> m_textures;
    VertexBuffer<MeshVertex, 3, 2, 3, 3> m_vertex_buffer;
};

} // namespace eb

#endif // EB_GRAPHICS_MESH_H
