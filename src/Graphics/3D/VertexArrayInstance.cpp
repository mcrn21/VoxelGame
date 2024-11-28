#include "VertexArrayInstance.h"
#include "../Common/DefaultShaders.h"
#include "../Common/RenderTarget.h"

namespace eb {

VertexArrayInstance::VertexArrayInstance()
    : Drawable{}
    , m_primitive_type{TRIANGLES}
{}

VertexArrayInstance::VertexArrayInstance(const std::vector<Vertex3D> &vertices,
                                         const std::vector<uint32_t> &indices,
                                         const Material &material)
    : Drawable{}
    , m_primitive_type{TRIANGLES}
{
    create(vertices, indices, material);
}

PrimitiveType VertexArrayInstance::getPrimitiveType() const
{
    return m_primitive_type;
}

void VertexArrayInstance::setPrimitiveType(PrimitiveType primitive_type)
{
    m_primitive_type = primitive_type;
}

const std::vector<Vertex3D> &VertexArrayInstance::getVertices() const
{
    return m_vertices;
}

const std::vector<uint32_t> &VertexArrayInstance::getIndices() const
{
    return m_indices;
}

const Material &VertexArrayInstance::getMaterial() const
{
    return m_material;
}

void VertexArrayInstance::setMaterial(const Material &material)
{
    m_material = material;
}

void VertexArrayInstance::create(const std::vector<Vertex3D> &vertices,
                                 const std::vector<uint32_t> &indices)
{
    m_vertices = vertices;
    m_indices = indices;
    m_vertex_array.create<Vertex3D, 3, 3, 4, 2>();
    m_vertex_array.setData(vertices, indices);
}

void VertexArrayInstance::create(const std::vector<Vertex3D> &vertices,
                                 const std::vector<uint32_t> &indices,
                                 const Material &material)
{
    create(vertices, indices);
    m_material = material;
}

void VertexArrayInstance::destroy()
{
    m_vertices.clear();
    m_indices.clear();
    m_material = Material{};
    m_vertex_array.destroy();
}

void VertexArrayInstance::draw(const RenderState &render_state) const
{
    render_state.shader->uniformMatrix("u_model", render_state.model_transform * getTransform());
    render_state.shader->uniformMaterial(m_material);
    m_vertex_array.draw(m_primitive_type);
}

} // namespace eb
