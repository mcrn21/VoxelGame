#ifndef EB_GRAPHICS_LINESBATCH_H
#define EB_GRAPHICS_LINESBATCH_H

#include "../EngineObject.h"
#include "Drawable.h"
#include "Transformable.h"
#include "VertexArray.h"

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

class LinesBatch : public EngineObject, public Drawable, public Transformable
{
public:
    LinesBatch(Engine *engine);
    ~LinesBatch() = default;

    float getLineWidth() const;
    void setLineWidth(float line_width);

    void create(const std::vector<LineVertex> &vertices, const std::vector<uint32_t> &indices);
    void cube(const glm::vec3 &from, const glm::vec3 &to, const glm::vec4 &color);

    void draw(const RenderTarget &render_target, const RenderState &render_state) const;

private:
    float m_line_width;
    VertexArray m_vertex_array;
};

} // namespace eb

#endif // EB_GRAPHICS_LINESBATCH_H
