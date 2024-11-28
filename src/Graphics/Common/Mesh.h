#ifndef EB_GRAPHICS_MESH_H
#define EB_GRAPHICS_MESH_H

#include "../3D/Material.h"
#include "../3D/Vertex3D.h"
#include "VertexArray.h"

#include <glm/glm.hpp>

#include <vector>

using namespace glm;

namespace eb {

class Mesh
{
public:
    Mesh();
    Mesh(const std::vector<Vertex3D> &vertices,
         const std::vector<uint32_t> &indices,
         const Material &material,
         const mat4 &transform = mat4{1.0f});
    ~Mesh();

    void create(const std::vector<Vertex3D> &vertices,
                const std::vector<uint32_t> &indices,
                const Material &material,
                const mat4 &transform = mat4{1.0f});
    void destroy();

    const std::vector<Vertex3D> &getVertices() const;
    const std::vector<uint32_t> &getIndices() const;
    const Material &getMaterial() const;

    const VertexArray &getVertexArray() const;

    const std::string &getName() const;
    void setName(const std::string &name);

    const mat4 &getTransform() const;

private:
    std::vector<Vertex3D> m_vertices;
    std::vector<uint32_t> m_indices;
    Material m_material;
    VertexArray m_vertex_array;
    std::string m_name;
    mat4 m_transform;
};

} // namespace eb

#endif // EB_GRAPHICS_MESH_H
