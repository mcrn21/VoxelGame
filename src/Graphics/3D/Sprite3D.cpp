#include "Sprite3D.h"
#include "../Common/DefaultShaders.h"
#include "../Common/RenderTarget.h"
#include "Vertex3D.h"

namespace eb {

Sprite3D::Sprite3D()
    : Drawable{}
{}

const Material &Sprite3D::getMaterial() const
{
    return m_material;
}

void Sprite3D::setMaterial(const Material &material, const i32vec4 &texture_rect)
{
    m_material = material;
    setTextureRect(texture_rect);
}

const i32vec4 &Sprite3D::getTextureRect() const
{
    return m_texture_rect;
}

void Sprite3D::setTextureRect(const i32vec4 &texture_rect)
{
    m_texture_rect = texture_rect;
    if (m_texture_rect == i32vec4{0} && m_material.diffuse_texture0)
        m_texture_rect = i32vec4{0,
                                 0,
                                 m_material.diffuse_texture0->getSize().x,
                                 m_material.diffuse_texture0->getSize().y};
    updateVertices();
}

void Sprite3D::draw(const RenderState &render_state) const
{
    render_state.shader->uniformMatrix("u_model", render_state.model_transform * getTransform());
    render_state.shader->uniformMaterial(m_material);
    m_vertex_array.draw();
}

void Sprite3D::updateVertices()
{
    if (!m_vertex_array.isValid())
        m_vertex_array.create<Vertex3D, 3, 3, 4, 2>();

    glm::vec4 uv{0.0f};
    if (m_material.diffuse_texture0)
        uv = m_material.diffuse_texture0->getUVRect(m_texture_rect);

    std::vector<Vertex3D> vertices
        = {{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, vec4{1.0f}, {uv.x, uv.w}},
           {{m_texture_rect.z, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, vec4{1.0f}, {uv.z, uv.w}},
           {{m_texture_rect.z, m_texture_rect.w, 0.0f}, {0.0f, 0.0f, 1.0f}, vec4{1.0f}, {uv.z, uv.y}},
           {{0.0f, m_texture_rect.w, 0.0f}, {0.0f, 0.0f, 1.0f}, vec4{1.0f}, {uv.x, uv.y}}};

    static const std::vector<uint32_t> indices = {0, 1, 3, 1, 2, 3};

    m_vertex_array.setData(vertices, indices);
}

} // namespace eb
