#ifndef EB_GRAPHICS_TEXTUREFBO_H
#define EB_GRAPHICS_TEXTUREFBO_H

#include <glm/glm.hpp>

using namespace glm;

namespace eb {

class TextureFBO
{
public:
    TextureFBO();
    ~TextureFBO() = default;

    void create(const i32vec2 &);

private:
    uint32_t m_fbo;
    bool m_valid;
    i32vec2 m_size;
};

} // namespace eb

#endif // EB_GRAPHICS_TEXTUREFBO_H
