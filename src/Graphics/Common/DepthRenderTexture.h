#ifndef EB_GRAPHICS_DEPTHRENDERTEXTURE_H
#define EB_GRAPHICS_DEPTHRENDERTEXTURE_H

#include "RenderTarget.h"
#include "Texture.h"

#include <string>

namespace eb {

class DepthRenderTexture : public RenderTarget
{
public:
    DepthRenderTexture();
    ~DepthRenderTexture();

    const Texture &getTexture() const;

    void create(const i32vec2 &size);
    bool isValid() const;
    void destroy();

    void clear(const vec4 &color = vec4{0.0f}) const;
    void display() const;

    void saveTofile(const std::string &file_name) const;

private:
    void setSize(const i32vec2 &size){};

private:
    Texture m_texture;
    uint32_t m_fbo;
    bool m_valid;
};

} // namespace eb

#endif // EB_GRAPHICS_DEPTHRENDERTEXTURE_H
