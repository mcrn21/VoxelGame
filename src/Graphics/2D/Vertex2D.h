#ifndef EB_GRAPHICS_2D_VERTEX2D_H
#define EB_GRAPHICS_2D_VERTEX2D_H

#include <glm/glm.hpp>

using namespace glm;

namespace eb {

struct Vertex2D
{
    vec3 position;
    vec4 color;
    vec2 tex_coords;
};

} // namespace eb

#endif // EB_GRAPHICS_2D_VERTEX2D_H
