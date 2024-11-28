#include "Mesh.h"

namespace eb {

Mesh::Mesh() {}

Mesh::Mesh(const std::vector<Vertex3D> &vertices,
           const std::vector<uint32_t> &indices,
           const Material &material,
           const mat4 &transform)
{
    create(vertices, indices, material, transform);
}

Mesh::~Mesh()
{
    destroy();
}

void Mesh::create(const std::vector<Vertex3D> &vertices,
                  const std::vector<uint32_t> &indices,
                  const Material &material,
                  const mat4 &transform)
{
    m_vertices = vertices;
    m_indices = indices;
    m_vertex_array.create<Vertex3D, 3, 3, 4, 2>();
    m_vertex_array.setData(vertices, indices);
    m_material = material;
    m_transform = transform;
}

void Mesh::destroy()
{
    m_vertices.clear();
    m_indices.clear();
    m_material = Material{};
    m_vertex_array.destroy();
}

const std::vector<Vertex3D> &Mesh::getVertices() const
{
    return m_vertices;
}

const std::vector<uint32_t> &Mesh::getIndices() const
{
    return m_indices;
}

const Material &Mesh::getMaterial() const
{
    return m_material;
}

const VertexArray &Mesh::getVertexArray() const
{
    return m_vertex_array;
}

const std::string &Mesh::getName() const
{
    return m_name;
}

void Mesh::setName(const std::string &name)
{
    m_name = name;
}

const mat4 &Mesh::getTransform() const
{
    return m_transform;
}

} // namespace eb
