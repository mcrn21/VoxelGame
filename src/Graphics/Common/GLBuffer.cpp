#include "GLBuffer.h"

namespace eb {

GLBuffer::GLBuffer(BufferType type, UsageType usage_type)
    : m_type{type}
    , m_usage_type{usage_type}
    , m_id{0}
    , m_valid{false}
    , m_size{0}
{}

GLBuffer::~GLBuffer()
{
    destroy();
}

BufferType GLBuffer::getType() const
{
    return m_type;
}

UsageType GLBuffer::getUsageType() const
{
    return m_usage_type;
}

uint32_t GLBuffer::getId() const
{
    return m_id;
}

bool GLBuffer::isValid() const
{
    return m_valid;
}

int32_t GLBuffer::getSize() const
{
    return m_size;
}

void GLBuffer::create(int32_t size)
{
    destroy();

    glGenBuffers(1, &m_id);
    glBindBuffer(m_type, m_id);
    glBufferData(m_type, size, nullptr, m_usage_type);
    glBindBuffer(m_type, 0);

    m_valid = true;
    m_size = size;
}

void GLBuffer::destroy()
{
    if (!m_valid)
        return;

    glDeleteBuffers(1, &m_id);

    m_id = 0;
    m_valid = false;
    m_size = 0;
}

void GLBuffer::bind(const GLBuffer &gl_buffer)
{
    if (!gl_buffer.m_valid)
        return;
    glBindBuffer(gl_buffer.m_type, gl_buffer.m_id);
}

void GLBuffer::bindToShader(const GLBuffer &gl_buffer, int32_t index)
{
    if (!gl_buffer.m_valid)
        return;
    glBindBufferBase(gl_buffer.m_type, index, gl_buffer.m_id);
}

void GLBuffer::unbind(const GLBuffer &gl_buffer)
{
    if (!gl_buffer.m_valid)
        return;
    glBindBuffer(gl_buffer.m_type, 0);
}

void GLBuffer::unbind(BufferType type)
{
    glBindBuffer(type, 0);
}

} // namespace eb
