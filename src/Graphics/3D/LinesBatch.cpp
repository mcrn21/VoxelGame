#include "LinesBatch.h"
#include "../Common/DefaultShaders.h"
#include "../Common/RenderTarget.h"

namespace eb {

LinesBatch::LinesBatch()
    : VertexArrayInstance{}
    , m_line_width{1.0f}
{
    setPrimitiveType(LINES);
}

float LinesBatch::getLineWidth() const
{
    return m_line_width;
}

void LinesBatch::setLineWidth(float line_width)
{
    m_line_width = line_width;
}

void LinesBatch::cube(const vec3 &from, const vec3 &to, const vec4 &color)
{
    std::vector<Vertex3D> vertices;

    // back
    vertices.push_back(Vertex3D{from, vec3{0.0f}, color});
    vertices.push_back(Vertex3D{{to.x, from.y, from.z}, vec3{0.0f}, color});
    vertices.push_back(Vertex3D{{to.x, to.y, from.z}, vec3{0.0f}, color});
    vertices.push_back(Vertex3D{{from.x, to.y, from.z}, vec3{0.0f}, color});

    // front
    vertices.push_back(Vertex3D{{from.x, from.y, to.z}, vec3{0.0f}, color});
    vertices.push_back(Vertex3D{{to.x, from.y, to.z}, vec3{0.0f}, color});
    vertices.push_back(Vertex3D{{to.x, to.y, to.z}, vec3{0.0f}, color});
    vertices.push_back(Vertex3D{{from.x, to.y, to.z}, vec3{0.0f}, color});

    static const std::vector<uint32_t> indices = {0, 1, 1, 2, 2, 3, 3, 0, // back
                                                  4, 5, 5, 6, 6, 7, 7, 4, // front
                                                  0, 4, 1, 5, 2, 6, 3, 7};

    create(vertices, indices);

    // m_vertex_array.setData(vertices, indices);
}

void LinesBatch::draw(const RenderState &render_state) const
{
    glLineWidth(m_line_width);
    VertexArrayInstance::draw(render_state);
}

} // namespace eb
