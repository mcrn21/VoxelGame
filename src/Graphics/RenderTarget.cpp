#include "RenderTarget.h"
#include "Drawable.h"

#include <GL/glew.h>
#include <glm/ext.hpp>

namespace eb {

RenderTarget::RenderTarget()
    : m_size{1}
    , m_viewport{1}
    , m_camera{std::make_shared<Camera>()}
    , m_2d_mat{1.0f}
{}

const glm::i32vec2 &RenderTarget::getSize() const
{
    return m_size;
}

void RenderTarget::setSize(const glm::i32vec2 &size)
{
    m_size = size;
    m_2d_mat = glm::ortho(0.0f,
                          static_cast<float>(m_size.x),
                          0.0f,
                          static_cast<float>(m_size.y),
                          -1.0f,
                          1.0f);
}

const glm::i32vec4 &RenderTarget::getViewport() const
{
    return m_viewport;
}

void RenderTarget::setViewport(const glm::i32vec4 &viewport)
{
    m_viewport = viewport;
    glViewport(m_viewport.x, m_viewport.y, m_viewport.z, m_viewport.w);
    // if (m_camera)
    //     m_camera->setViewportSize(glm::vec2{m_viewport.z, m_viewport.w});
}

std::shared_ptr<Camera> RenderTarget::getCamera() const
{
    return m_camera;
}

void RenderTarget::setCamera(const std::shared_ptr<Camera> &camera)
{
    if (!camera)
        return;

    m_camera = camera;
    // m_camera->setViewportSize(glm::vec2{m_viewport.z, m_viewport.w});
}

const glm::mat4 &RenderTarget::get2DMat() const
{
    return m_2d_mat;
}

void RenderTarget::setClearColor(const glm::vec4 &color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void RenderTarget::draw(const Drawable &drawable) const
{
    RenderState render_state;
    drawable.draw(*this, render_state);
}

void RenderTarget::draw(const VertexBufferBase &vertex_buffer,
                        const RenderState &render_state,
                        VertexBufferBase::PrimitiveType primitive_type) const
{
    if (render_state.shader) {
        Shader::use(render_state.shader);
        render_state.shader->uniformMatrix("u_model", render_state.transform);
        render_state.shader->uniformMatrix("u_view", m_camera->getViewMat());
        render_state.shader->uniformMatrix("u_projection", m_camera->getProjectionMat());
    }

    if (render_state.texture)
        Texture::bind(render_state.texture);

    vertex_buffer.draw(primitive_type);

    if (render_state.texture)
        Texture::bind(nullptr);

    if (render_state.shader)
        Shader::use(nullptr);
}

void RenderTarget::draw2D(const VertexBufferBase &vertex_buffer,
                          const RenderState &render_state,
                          VertexBufferBase::PrimitiveType primitive_type) const
{
    if (render_state.shader) {
        Shader::use(render_state.shader);
        render_state.shader->uniformMatrix("u_model", render_state.transform);
        render_state.shader->uniformMatrix("u_projection", m_2d_mat);
    }

    if (render_state.texture)
        Texture::bind(render_state.texture);

    vertex_buffer.draw(primitive_type);

    if (render_state.texture)
        Texture::bind(nullptr);

    if (render_state.shader)
        Shader::use(nullptr);
}

} // namespace eb
