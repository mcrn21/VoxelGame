#ifndef EB_GRAPHICS_TEXTURE_H
#define EB_GRAPHICS_TEXTURE_H

#include <GL/glew.h>
#include <glm/glm.hpp>

using namespace glm;

namespace eb {

class Texture
{
public:
    enum Format {
        LUMINANCE = GL_LUMINANCE,
        LUMINANCE_ALPHA = GL_LUMINANCE_ALPHA,
        RGB = GL_RGB,
        RGBA = GL_RGBA,
        DEPTH = GL_DEPTH_COMPONENT
    };

    Texture();
    Texture(uint32_t id, const i32vec2 &size);
    ~Texture();

    i32vec2 getSize() const;
    vec4 getUVRect(const i32vec4 &sub_rect) const;

    void create(const i32vec2 &size, Format format, const uint8_t *data);

    bool isValid() const;
    void destroy();

    static void bind(const Texture *texture);

private:
    uint32_t m_id;
    i32vec2 m_size;
    bool m_valid;
};

} // namespace eb

#endif // EB_GRAPHICS_TEXTURE_H
