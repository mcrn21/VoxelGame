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
    m_vertex_buffer.create(vertices.size(), indices.size());
    m_vertex_buffer.update(vertices, indices);
}

void Mesh::destroy()
{
    m_vertices.clear();
    m_indices.clear();
    m_textures.clear();
    m_vertex_buffer.destroy();
}

void Mesh::draw(const RenderTarget &render_target, const RenderState &render_state) const
{
    RenderState new_render_state = render_state;
    new_render_state.transform *= getTransform();
    new_render_state.shader = DefaultShaders::getMesh().get();
    if (!m_textures.empty())
        new_render_state.texture = m_textures[0].get();
    render_target.draw(m_vertex_buffer, new_render_state);
}

} // namespace eb
