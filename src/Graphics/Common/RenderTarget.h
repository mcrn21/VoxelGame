#ifndef EB_GRAPHICS_RENDERTARGET_H
#define EB_GRAPHICS_RENDERTARGET_H

#include <glm/glm.hpp>

using namespace glm;

namespace eb {

class Drawable;

class RenderTarget
{
public:
    RenderTarget();
    virtual ~RenderTarget() = default;

    const i32vec2 &getSize() const;
    void setSize(const i32vec2 &size);

    const i32vec4 &getViewport() const;
    void setViewport(const i32vec4 &viewport);

    virtual void clear(const vec4 &color = vec4{0.0f}) const;
    virtual void display() const = 0;

protected:
    void applyViewport() const;
    void applyClearColor(const vec4 &color) const;

private:
    i32vec2 m_size;
    i32vec4 m_viewport;
};

} // namespace eb

#endif // EB_GRAPHICS_RENDERTARGET_H
