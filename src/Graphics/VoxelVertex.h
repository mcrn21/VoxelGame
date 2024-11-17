#ifndef EB_GRAPHICS_VOXELVERTEX_H
#define EB_GRAPHICS_VOXELVERTEX_H

#include <glm/glm.hpp>

namespace eb {

struct VoxelVertex
{
    VoxelVertex()
        : position{0.0f}
        , color{1.0f}
        , tex_coords{0.0f}
        , light{1.0f}
    {}
    VoxelVertex(glm::vec3 position, glm::vec4 color, glm::vec2 tex_coords, float light)
        : position{position}
        , color{color}
        , tex_coords{tex_coords}
        , light{light}
    {}
    ~VoxelVertex() = default;

    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 tex_coords;
    float light;
};

} // namespace eb

#endif // EB_GRAPHICS_VOXELVERTEX_H
