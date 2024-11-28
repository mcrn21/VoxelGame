#ifndef EB_GRAPHICS_3D_MATERIAL_H
#define EB_GRAPHICS_3D_MATERIAL_H

#include "../Common/Texture.h"

#include <glm/glm.hpp>

#include <memory>

using namespace glm;

namespace eb {

struct Material
{
    std::shared_ptr<Texture> diffuse_texture0;
    vec4 color{1.0f};
    vec3 specular = {0.0f, 0.0f, 0.0f};
    float shininess = 32.0f;
};

} // namespace eb

#endif // EB_GRAPHICS_3D_MATERIAL_H
