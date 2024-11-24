#ifndef EB_GRAPHICS_VERTEXARRAY_H
#define EB_GRAPHICS_VERTEXARRAY_H

#include "GlBuffer.h"

#include <spdlog/spdlog.h>

#include <array>
#include <vector>

namespace eb {

class VertexArray
{
public:
    VertexArray(UsageType usage_type = DYNAMIC);
    ~VertexArray();

    UsageType getUsageType() const;
    int32_t getVertexCount() const;
    int32_t getIndicesCount() const;
    bool isValid() const;

    template<typename V, int32_t... Is>
    void create()
    {
        destroy();

        glGenVertexArrays(1, &m_vao);

        m_vbo.create();
        m_ebo.create();

        glBindVertexArray(m_vao);

        GlBuffer::bind(m_vbo);
        GlBuffer::bind(m_ebo);

        std::array<int32_t, sizeof...(Is)> attrs = {Is...};
        int32_t byte_offset = 0;
        for (int32_t i = 0; i < attrs.size(); ++i) {
            glVertexAttribPointer(i,
                                  attrs[i],
                                  GL_FLOAT,
                                  GL_FALSE,
                                  sizeof(V),
                                  (GLvoid *) (byte_offset * sizeof(GLfloat)));
            glEnableVertexAttribArray(i);
            byte_offset += attrs[i];
        }

        GlBuffer::unbind(m_vbo);
        glBindVertexArray(0);
        GlBuffer::unbind(m_ebo);

        m_vertex_count = 0;
        m_indices_count = 0;
        m_valid = true;
    }

    void destroy();

    template<typename V>
    void setData(const std::vector<V> &vertices, const std::vector<uint32_t> &indices)
    {
        if (!m_valid)
            return;

        m_vbo.setData(vertices.data(), vertices.size() * sizeof(V));
        m_vertex_count = vertices.size();

        m_ebo.setData(indices.data(), indices.size() * sizeof(uint32_t));
        m_indices_count = indices.size();
    }

    void draw(PrimitiveType primitive_type = TRIANGLES) const;

private:
    UsageType m_usage_type;
    GlBuffer m_vbo;
    GlBuffer m_ebo;
    uint32_t m_vao;
    int32_t m_vertex_count;
    int32_t m_indices_count;
    bool m_valid;
};

} // namespace eb

#endif // EB_GRAPHICS_VERTEXARRAY_H
