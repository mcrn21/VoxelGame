#ifndef EB_GRAPHICS_2D_SPRITE2D_H
#define EB_GRAPHICS_2D_SPRITE2D_H

#include "../Common/Drawable.h"
#include "../Common/Texture.h"
#include "../Common/VertexArray.h"

#include <glm/glm.hpp>

#include <memory>

using namespace glm;

namespace eb {

class Sprite2D : public Drawable
{
public:
    Sprite2D();
    ~Sprite2D() = default;

    std::shared_ptr<Texture> getTexture() const;
    void setTexture(const std::shared_ptr<Texture> &texture,
                    const glm::i32vec4 &texture_rect = glm::i32vec4{0});

    const i32vec4 &getTextureRect() const;
    void setTextureRect(const i32vec4 &texture_rect);

    void draw(const RenderState &render_state) const;

protected:
    void updateVertices();

private:
    std::shared_ptr<Texture> m_texture;
    i32vec4 m_texture_rect;
    VertexArray m_vertex_array;
};

} // namespace eb

#endif // EB_GRAPHICS_2D_SPRITE2D_H
