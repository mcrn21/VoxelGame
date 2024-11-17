#ifndef EB_GRAPHICS_RENDERTARGET_H
#define EB_GRAPHICS_RENDERTARGET_H

#include "Camera.h"
#include "RenderState.h"
#include "VertexBuffer.h"

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

    const glm::mat4 &get2DMat() const;

    void setClearColor(const glm::vec4 &color);

    void draw(const Drawable &drawable) const;
    void draw(const VertexBufferBase &vertex_buffer,
              const RenderState &render_state = {},
              VertexBufferBase::PrimitiveType primitive_type = VertexBufferBase::TRIANGLES) const;
    void draw2D(const VertexBufferBase &vertex_buffer,
                const RenderState &render_state = {},
                VertexBufferBase::PrimitiveType primitive_type = VertexBufferBase::TRIANGLES) const;

private:
    glm::i32vec2 m_size;
    glm::i32vec4 m_viewport;
    std::shared_ptr<Camera> m_camera;
    glm::mat4 m_2d_mat;
};

} // namespace eb

#endif // EB_GRAPHICS_RENDERTARGET_H
