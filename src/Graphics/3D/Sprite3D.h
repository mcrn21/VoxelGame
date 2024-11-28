#ifndef EB_GRAPHICS_3D_SPRITE3D_H
#define EB_GRAPHICS_3D_SPRITE3D_H

#include "../Common/Drawable.h"
#include "../Common/VertexArray.h"
#include "Material.h"

#include <glm/glm.hpp>

namespace eb {

class Sprite3D : public Drawable
{
public:
    Sprite3D();
    ~Sprite3D() = default;

    const Material &getMaterial() const;
    void setMaterial(const Material &material, const i32vec4 &texture_rect = i32vec4{0});

    const i32vec4 &getTextureRect() const;
    void setTextureRect(const i32vec4 &texture_rect);

    void draw(const RenderState &render_state) const;

private:
    void updateVertices();

private:
    Material m_material;
    i32vec4 m_texture_rect;
    VertexArray m_vertex_array;
};

} // namespace eb

#endif // EB_GRAPHICS_3D_SPRITE3D_H
