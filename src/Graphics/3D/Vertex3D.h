#ifndef EB_GRAPHICS_3D_VERTEX3D_H
#define EB_GRAPHICS_3D_VERTEX3D_H

#include <glm/glm.hpp>

using namespace glm;

namespace eb {

struct Vertex3D
{
    vec3 position;
    vec3 normal;
    vec4 color;
    vec2 tex_coords;
};

} // namespace eb

#endif // EB_GRAPHICS_3D_VERTEX3D_H
