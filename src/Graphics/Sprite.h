#ifndef EB_GRAPHICS_SPRITE_H
#define EB_GRAPHICS_SPRITE_H

#include "../EngineObject.h"
#include "Drawable.h"
#include "Shader.h"
#include "Texture.h"
#include "Transformable.h"
#include "VertexBuffer.h"

#include <glm/glm.hpp>

#include <memory>

namespace eb {

class Sprite : public EngineObject, public Drawable, public Transformable
{
public:
    enum DrawType { DRAW_3D, DRAW_2D };

    Sprite(Engine *engine);
    Sprite(Engine *engine,
           const std::shared_ptr<Texture> &texture,
           const glm::i32vec4 &texture_rect = glm::i32vec4{0});
    ~Sprite() = default;

    DrawType getDrawType() const;
    void setDrawType(DrawType draw_type);

    std::shared_ptr<Texture> getTexture() const;
    void setTexture(const std::shared_ptr<Texture> &texture,
                    const glm::i32vec4 &texture_rect = glm::i32vec4{0});

    std::shared_ptr<Shader> getShader() const;
    void setShader(const std::shared_ptr<Shader> &shader);

    const glm::i32vec4 &getTextureRect() const;
    void setTextureRect(const glm::i32vec4 &texture_rect);

    void draw(const RenderTarget &render_target) const;

private:
    void updateVertices();

private:
    DrawType m_draw_type;
    std::shared_ptr<Texture> m_texture;
    std::shared_ptr<Shader> m_shader;
    glm::i32vec4 m_texture_rect;
    DefaultVertexBuffer m_vertex_buffer;
};

} // namespace eb

#endif // EB_GRAPHICS_SPRITE_H
