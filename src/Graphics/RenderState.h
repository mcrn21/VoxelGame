#ifndef EB_GRAPHICS_RENDERSTATE_H
#define EB_GRAPHICS_RENDERSTATE_H

#include "Shader.h"
#include "Texture.h"

#include <glm/glm.hpp>

namespace eb {

struct RenderState
{
    glm::mat4 transform{1.0f};
    Texture *texture = nullptr;
    Shader *shader = nullptr;
};

} // namespace eb

#endif // EB_GRAPHICS_RENDERSTATE_H
