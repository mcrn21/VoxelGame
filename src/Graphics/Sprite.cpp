#include "Sprite.h"
#include "DefaultShaders.h"
#include "Shader.h"
#include "Vertex.h"

namespace eb {

Sprite::Sprite(Engine *engine)
    : EngineObject{engine}
    , Drawable{}
    , Transformable{}
    , m_draw_type{DRAW_3D}
    , m_texture{nullptr}
    , m_shader{DefaultShaders::getMain()}
{
    m_vertex_array.create<Vertex, 3, 4, 2>();
}

Sprite::Sprite(Engine *engine,
               const std::shared_ptr<Texture> &texture,
               const glm::i32vec4 &texture_rect)
    : EngineObject{engine}
    , Drawable{}
    , Transformable{}
    , m_draw_type{DRAW_3D}
    , m_shader{DefaultShaders::getMain()}
{
    m_vertex_array.create<Vertex, 3, 4, 2>();
    setTexture(texture, texture_rect);
}

Sprite::DrawType Sprite::getDrawType() const
{
    return m_draw_type;
}

void Sprite::setDrawType(DrawType draw_type)
{
    m_draw_type = draw_type;
    if (m_shader
        && (m_shader->getId() == DefaultShaders::getMain()->getId()
            || m_shader->getId() == DefaultShaders::getMain2D()->getId())) {
        m_shader = m_draw_type == DRAW_3D ? DefaultShaders::getMain() : DefaultShaders::getMain2D();
    }
}

std::shared_ptr<Texture> Sprite::getTexture() const
{
    return m_texture;
}

void Sprite::setTexture(const std::shared_ptr<Texture> &texture, const glm::i32vec4 &texture_rect)
{
    m_texture = texture;
    m_texture_rect = texture_rect;
    if (m_texture_rect == glm::i32vec4{0} && m_texture)
        m_texture_rect = glm::i32vec4{0, 0, m_texture->getSize().x, m_texture->getSize().y};
    updateVertices();
}

std::shared_ptr<Shader> Sprite::getShader() const
{
    return m_shader;
}

void Sprite::setShader(const std::shared_ptr<Shader> &shader)
{
    m_shader = shader ? shader
                      : (m_draw_type == DRAW_3D ? DefaultShaders::getMain()
                                                : DefaultShaders::getMain2D());
}

const glm::i32vec4 &Sprite::getTextureRect() const
{
    return m_texture_rect;
}

void Sprite::setTextureRect(const glm::i32vec4 &texture_rect)
{
    m_texture_rect = texture_rect;
    updateVertices();
}

void Sprite::draw(const RenderTarget &render_target, const RenderState &render_state) const
{
    RenderState new_render_state = render_state;
    new_render_state.transform *= getTransform();
    new_render_state.shader = m_shader.get();
    new_render_state.texture = m_texture.get();
    if (m_draw_type == DRAW_3D)
        render_target.draw3D(m_vertex_array, new_render_state);
    else
        render_target.draw2D(m_vertex_array, new_render_state);
}

void Sprite::updateVertices()
{
    if (m_texture == nullptr)
        return;

    glm::vec4 uv = m_texture->getUVRect(m_texture_rect);

    std::vector<Vertex> vertices = {{{0.0f, 0.0f, 0.0f}, {uv.x, uv.w}},
                                    {{m_texture_rect.z, 0.0f, 0.0f}, {uv.z, uv.w}},
                                    {{m_texture_rect.z, m_texture_rect.w, 0.0f}, {uv.z, uv.y}},
                                    {{0.0f, m_texture_rect.w, 0.0f}, {uv.x, uv.y}}};

    static const std::vector<uint32_t> indices = {0, 1, 3, 1, 2, 3};

    m_vertex_array.setData(vertices, indices);
}

} // namespace eb
