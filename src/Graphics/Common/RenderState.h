#ifndef EB_GRAPHICS_RENDERSTATE_H
#define EB_GRAPHICS_RENDERSTATE_H

#include "Shader.h"

#include <glm/glm.hpp>

using namespace glm;

namespace eb {

struct RenderState
{
    mat4 model_transform{1.0f};
    Shader *shader;
};

} // namespace eb

#endif // EB_GRAPHICS_RENDERSTATE_H
