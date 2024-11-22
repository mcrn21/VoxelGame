#ifndef EB_GRAPHICS_LINESBATCH_H
#define EB_GRAPHICS_LINESBATCH_H

#include "../EngineObject.h"
#include "Drawable.h"
#include "LineVertex.h"
#include "Transformable.h"
#include "VertexBuffer.h"

namespace eb {

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
    VertexBuffer<LineVertex, 4> m_vertex_buffer;
};

} // namespace eb

#endif // EB_GRAPHICS_LINESBATCH_H
