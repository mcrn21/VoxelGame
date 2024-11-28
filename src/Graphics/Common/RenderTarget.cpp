#include "RenderTarget.h"

#include <GL/glew.h>

namespace eb {

RenderTarget::RenderTarget()
    : m_size{1}
    , m_viewport{1}
{}

const i32vec2 &RenderTarget::getSize() const
{
    return m_size;
}

void RenderTarget::setSize(const i32vec2 &size)
{
    m_size = size;
}

const i32vec4 &RenderTarget::getViewport() const
{
    return m_viewport;
}

void RenderTarget::setViewport(const i32vec4 &viewport)
{
    m_viewport = viewport;
}

void RenderTarget::clear(const vec4 &color) const
{
    applyViewport();
    applyClearColor(color);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderTarget::applyViewport() const
{
    glViewport(m_viewport.x, m_viewport.y, m_viewport.z, m_viewport.w);
}

void RenderTarget::applyClearColor(const vec4 &color) const
{
    glClearColor(color.r, color.g, color.b, color.a);
}

} // namespace eb
