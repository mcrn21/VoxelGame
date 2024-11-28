#include "VertexArray.h"

namespace eb {

VertexArray::VertexArray(UsageType usage_type)
    : m_usage_type{usage_type}
    , m_vbo{ARRAY_BUFFER, usage_type}
    , m_ebo{ELEMENT_ARRAY_BUFFER, usage_type}
    , m_vao{0}
    , m_vertex_count{0}
    , m_indices_count{0}
    , m_valid{false}
{}

VertexArray::~VertexArray()
{
    destroy();
}

UsageType VertexArray::getUsageType() const
{
    return m_usage_type;
}

int32_t VertexArray::getVertexCount() const
{
    return getVertexCount();
}

int32_t VertexArray::getIndicesCount() const
{
    return m_indices_count;
}

bool VertexArray::isValid() const
{
    return m_valid;
}

void VertexArray::destroy()
{
    if (m_valid)
        return;

    m_vbo.destroy();
    m_ebo.destroy();
    glDeleteVertexArrays(1, &m_vao);

    m_vao = 0;
    m_vertex_count = 0;
    m_indices_count = 0;
    m_valid = false;
}

void VertexArray::draw(PrimitiveType primitive_type) const
{
    if (m_valid) {
        glBindVertexArray(m_vao);
        glDrawElements(primitive_type, m_indices_count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

} // namespace eb
