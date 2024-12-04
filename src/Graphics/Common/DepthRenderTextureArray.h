#ifndef EB_GRAPHICS_DEPTHRENDERTEXTUREARRAY_H
#define EB_GRAPHICS_DEPTHRENDERTEXTUREARRAY_H

#include "RenderTarget.h"

#include <string>

namespace eb {

class DepthRenderTextureArray : public RenderTarget
{
public:
    DepthRenderTextureArray();
    ~DepthRenderTextureArray();

    uint32_t getTexture() const;

    void create(const i32vec2 &size, int32_t layers = 3);
    bool isValid() const;
    void destroy();

    void clear(const vec4 &color = vec4{0.0f}) const;
    void display() const;

    void saveTofile(const std::string &file_name) const;

private:
    void setSize(const i32vec2 &size){};

private:
    uint32_t m_texture;
    uint32_t m_fbo;
    int32_t m_layers;
    bool m_valid;
};

} // namespace eb

#endif // EB_GRAPHICS_DEPTHRENDERTEXTUREARRAY_H
