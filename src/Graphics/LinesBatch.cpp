#include "LinesBatch.h"
#include "DefaultShaders.h"

namespace eb {

LinesBatch::LinesBatch(Engine *engine)
    : EngineObject{engine}
    , m_line_width{1.0f}
{}

float LinesBatch::getLineWidth() const
{
    return m_line_width;
}

void LinesBatch::setLineWidth(float line_width)
{
    m_line_width = line_width;
}

void LinesBatch::create(const std::vector<LineVertex> &vertices)
{
    m_vertex_buffer.destroy();
    m_vertex_buffer.create(vertices);
}

void LinesBatch::cube(const glm::vec3 &from, const glm::vec3 &to, const glm::vec4 &color)
{
    std::vector<LineVertex> vertices;

    // back
    vertices.push_back(LineVertex{from, color});
    vertices.push_back(LineVertex{{to.x, from.y, from.z}, color});

    vertices.push_back(LineVertex{{to.x, from.y, from.z}, color});
    vertices.push_back(LineVertex{{to.x, to.y, from.z}, color});

    vertices.push_back(LineVertex{{to.x, to.y, from.z}, color});
    vertices.push_back(LineVertex{{from.x, to.y, from.z}, color});

    vertices.push_back(LineVertex{{from.x, to.y, from.z}, color});
    vertices.push_back(LineVertex{{from.x, from.y, from.z}, color});

    // front
    vertices.push_back(LineVertex{{from.x, from.y, to.z}, color});
    vertices.push_back(LineVertex{{to.x, from.y, to.z}, color});

    vertices.push_back(LineVertex{{to.x, from.y, to.z}, color});
    vertices.push_back(LineVertex{{to.x, to.y, to.z}, color});

    vertices.push_back(LineVertex{{to.x, to.y, to.z}, color});
    vertices.push_back(LineVertex{{from.x, to.y, to.z}, color});

    vertices.push_back(LineVertex{{from.x, to.y, to.z}, color});
    vertices.push_back(LineVertex{{from.x, from.y, to.z}, color});

    //sides
    vertices.push_back(LineVertex{{from.x, from.y, from.z}, color});
    vertices.push_back(LineVertex{{from.x, from.y, to.z}, color});

    vertices.push_back(LineVertex{{to.x, from.y, from.z}, color});
    vertices.push_back(LineVertex{{to.x, from.y, to.z}, color});

    vertices.push_back(LineVertex{{to.x, to.y, from.z}, color});
    vertices.push_back(LineVertex{{to.x, to.y, to.z}, color});

    vertices.push_back(LineVertex{{from.x, to.y, from.z}, color});
    vertices.push_back(LineVertex{{from.x, to.y, to.z}, color});

    create(vertices);
}

void LinesBatch::draw(const RenderTarget &render_target) const
{
    RenderState render_state;
    render_state.transform = getTransform();
    render_state.shader = DefaultShaders::getLines().get();
    glLineWidth(m_line_width);
    render_target.draw(m_vertex_buffer, render_state, VertexBufferBase::LINES);
}

} // namespace eb
