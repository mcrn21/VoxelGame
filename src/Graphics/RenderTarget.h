#ifndef EB_GRAPHICS_RENDERTARGET_H
#define EB_GRAPHICS_RENDERTARGET_H

#include "Camera.h"
#include "Lights.h"
#include "RenderState.h"
#include "VertexArray.h"

#include <glm/glm.hpp>

#include <memory>

namespace eb {

class Drawable;

class RenderTarget
{
public:
    RenderTarget();
    virtual ~RenderTarget() = default;

    const glm::i32vec2 &getSize() const;
    void setSize(const glm::i32vec2 &size);

    const glm::i32vec4 &getViewport() const;
    void setViewport(const glm::i32vec4 &viewport);

    std::shared_ptr<Camera> getCamera() const;
    void setCamera(const std::shared_ptr<Camera> &camera);

    std::shared_ptr<Lights> getLights() const;
    void setLights(const std::shared_ptr<Lights> &lights);

    const glm::mat4 &get2DMat() const;

    void setClearColor(const glm::vec4 &color);

    void draw(const Drawable &drawable) const;
    void draw3D(const VertexArray &vertex_array,
                const RenderState &render_state = {},
                PrimitiveType primitive_type = TRIANGLES) const;
    void draw2D(const VertexArray &vertex_array,
                const RenderState &render_state = {},
                PrimitiveType primitive_type = TRIANGLES) const;

private:
    glm::i32vec2 m_size;
    glm::i32vec4 m_viewport;
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<Lights> m_lights;
    glm::mat4 m_2d_mat;
};

} // namespace eb

#endif // EB_GRAPHICS_RENDERTARGET_H
