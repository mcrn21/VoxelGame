#include "Mesh.h"
#include "DefaultShaders.h"

namespace eb {

Mesh::Mesh()
    : Drawable{}
    , Transformable{}
{}

Mesh::Mesh(const std::vector<MeshVertex> &vertices,
           const std::vector<uint32_t> &indices,
           const std::vector<std::shared_ptr<Texture>> &textures)
    : Drawable{}
    , Transformable{}
{
    create(vertices, indices, textures);
}

void Mesh::create(const std::vector<MeshVertex> &vertices,
                  const std::vector<uint32_t> &indices,
                  const std::vector<std::shared_ptr<Texture>> &textures)
{
    m_vertices = vertices;
    m_indices = indices;
    m_textures = textures;
    m_vertex_array.create<MeshVertex, 3, 3, 2, 3, 3>();
    m_vertex_array.setData(vertices, indices);
}

void Mesh::destroy()
{
    m_vertices.clear();
    m_indices.clear();
    m_textures.clear();
    m_vertex_array.destroy();
}

void Mesh::draw(const RenderTarget &render_target, const RenderState &render_state) const
{
    RenderState new_render_state = render_state;
    new_render_state.transform *= getTransform();
    new_render_state.shader = DefaultShaders::getMesh().get();
    if (!m_textures.empty())
        new_render_state.texture = m_textures[0].get();
    render_target.draw3D(m_vertex_array, new_render_state);
}

} // namespace eb
