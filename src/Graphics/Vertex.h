#ifndef EB_GRAPHICS_VERTEX_H
#define EB_GRAPHICS_VERTEX_H

#include <glm/glm.hpp>

namespace eb {

struct Vertex
{
    Vertex()
        : position{0.0f}
        , color{1.0f}
        , tex_coords{0.0f}
    {}
    Vertex(glm::vec3 position)
        : position{position}
        , color{1.0f}
        , tex_coords{0.0f}
    {}
    Vertex(glm::vec3 position, glm::vec2 tex_coords)
        : position{position}
        , color{1.0f}
        , tex_coords{tex_coords}
    {}
    Vertex(glm::vec3 position, glm::vec4 color)
        : position{position}
        , color{color}
        , tex_coords{0.0f}
    {}
    Vertex(glm::vec3 position, glm::vec4 color, glm::vec2 tex_coords)
        : position{position}
        , color{color}
        , tex_coords{tex_coords}
    {}
    ~Vertex() = default;

    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 tex_coords;
};

} // namespace eb

#endif // EB_GRAPHICS_VERTEX_H
