#include "LinesBatch.h"
#include "DefaultShaders.h"

namespace eb {

LinesBatch::LinesBatch(Engine *engine)
    : EngineObject{engine}
    , m_line_width{1.0f}
{
    m_vertex_array.create<LineVertex, 3, 4>();
}

float LinesBatch::getLineWidth() const
{
    return m_line_width;
}

void LinesBatch::setLineWidth(float line_width)
{
    m_line_width = line_width;
}

void LinesBatch::create(const std::vector<LineVertex> &vertices,
                        const std::vector<uint32_t> &indices)
{
    m_vertex_array.setData(vertices, indices);
}

void LinesBatch::cube(const glm::vec3 &from, const glm::vec3 &to, const glm::vec4 &color)
{
    std::vector<LineVertex> vertices;

    // back
    vertices.push_back(LineVertex{from, color});
    vertices.push_back(LineVertex{{to.x, from.y, from.z}, color});
    vertices.push_back(LineVertex{{to.x, to.y, from.z}, color});
    vertices.push_back(LineVertex{{from.x, to.y, from.z}, color});

    // front
    vertices.push_back(LineVertex{{from.x, from.y, to.z}, color});
    vertices.push_back(LineVertex{{to.x, from.y, to.z}, color});
    vertices.push_back(LineVertex{{to.x, to.y, to.z}, color});
    vertices.push_back(LineVertex{{from.x, to.y, to.z}, color});

    static const std::vector<uint32_t> indices = {0, 1, 1, 2, 2, 3, 3, 0, // back
                                                  4, 5, 5, 6, 6, 7, 7, 4, // front
                                                  0, 4, 1, 5, 2, 6, 3, 7};

    m_vertex_array.setData(vertices, indices);
}

void LinesBatch::draw(const RenderTarget &render_target, const RenderState &render_state) const
{
    RenderState new_render_state = render_state;
    new_render_state.transform *= getTransform();
    new_render_state.shader = DefaultShaders::getLines().get();
    glLineWidth(m_line_width);
    render_target.draw3D(m_vertex_array, new_render_state, LINES);
}

} // namespace eb
