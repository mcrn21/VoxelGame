#ifndef EB_GRAPHICS_GLBUFFER_H
#define EB_GRAPHICS_GLBUFFER_H

#include "Enums.h"

#include <GL/glew.h>

namespace eb {

class GlBuffer
{
public:
    GlBuffer(BufferType type, UsageType usage_type = DYNAMIC);
    ~GlBuffer();

    BufferType getType() const;
    UsageType getUsageType() const;
    uint32_t getId() const;
    bool isValid() const;
    int32_t getSize() const;

    void create(int32_t size = 0);
    void destroy();

    template<typename T>
    void setData(const T *value, int32_t size = sizeof(T), int32_t offset = 0)
    {
        if (!m_valid)
            return;

        glBindBuffer(m_type, m_id);

        if ((size + offset) > m_size) {
            glBufferData(m_type, size + offset, nullptr, m_usage_type);
            m_size = size;
        }

        glBufferSubData(m_type, offset, size, value);

        glBindBuffer(m_type, 0);
    }

    static void bind(const GlBuffer &gl_buffer);
    static void bindToShader(const GlBuffer &gl_buffer, int32_t index);
    static void unbind(const GlBuffer &gl_buffer);
    static void unbind(BufferType type);

private:
    BufferType m_type;
    UsageType m_usage_type;
    uint32_t m_id;
    bool m_valid;
    int32_t m_size;
};

} // namespace eb

#endif // EB_GRAPHICS_GLBUFFER_H
