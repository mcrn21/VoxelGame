#ifndef EB_GRAPHICS_LINEVERTEX_H
#define EB_GRAPHICS_LINEVERTEX_H

#include <glm/glm.hpp>

namespace eb {

struct LineVertex
{
    LineVertex()
        : position{0.0f}
        , color{1.0f}
    {}
    LineVertex(glm::vec3 position, glm::vec4 color)
        : position{position}
        , color{color}
    {}
    ~LineVertex() = default;

    glm::vec3 position;
    glm::vec4 color;
};

} // namespace eb

#endif // EB_GRAPHICS_LINEVERTEX_H
