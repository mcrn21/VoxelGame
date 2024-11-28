#include "Sprite2D.h"
#include "../Common/DefaultShaders.h"
#include "../Common/RenderTarget.h"
#include "Vertex2D.h"

namespace eb {

Sprite2D::Sprite2D()
    : Drawable{}
{}

std::shared_ptr<Texture> Sprite2D::getTexture() const
{
    return m_texture;
}

void Sprite2D::setTexture(const std::shared_ptr<Texture> &texture, const i32vec4 &texture_rect)
{
    m_texture = texture;
    setTextureRect(texture_rect);
}

const i32vec4 &Sprite2D::getTextureRect() const
{
    return m_texture_rect;
}

void Sprite2D::setTextureRect(const i32vec4 &texture_rect)
{
    m_texture_rect = texture_rect;
    if (m_texture_rect == i32vec4{0} && m_texture)
        m_texture_rect = i32vec4{0, 0, m_texture->getSize().x, m_texture->getSize().y};
    updateVertices();
}

void Sprite2D::draw(const RenderState &render_state) const
{
    render_state.shader->uniformMatrix("u_model", render_state.model_transform * getTransform());
    Texture::bind(m_texture.get());
    m_vertex_array.draw();
}

void Sprite2D::updateVertices()
{
    if (!m_vertex_array.isValid())
        m_vertex_array.create<Vertex2D, 3, 4, 2>();

    glm::vec4 uv{0.0f};
    if (m_texture)
        uv = m_texture->getUVRect(m_texture_rect);

    std::vector<Vertex2D> vertices = {{{0.0f, 0.0f, 0.0f}, vec4{1.0f}, {uv.x, uv.w}},
                                      {{m_texture_rect.z, 0.0f, 0.0f}, vec4{1.0f}, {uv.z, uv.w}},
                                      {{m_texture_rect.z, m_texture_rect.w, 0.0f},
                                       vec4{1.0f},
                                       {uv.z, uv.y}},
                                      {{0.0f, m_texture_rect.w, 0.0f}, vec4{1.0f}, {uv.x, uv.y}}};

    static const std::vector<uint32_t> indices = {0, 1, 3, 1, 2, 3};

    m_vertex_array.setData(vertices, indices);
}

} // namespace eb
