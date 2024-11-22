#ifndef EB_GRAPHICS_VERTEXBUFFER_H
#define EB_GRAPHICS_VERTEXBUFFER_H

#include "Vertex.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include <vector>

namespace eb {

class VertexBufferBase
{
public:
    enum UsageType { STATIC = GL_STATIC_DRAW, DYNAMIC = GL_DYNAMIC_DRAW, STREAM = GL_STREAM_DRAW };
    enum PrimitiveType { TRIANGLES = GL_TRIANGLES, LINES = GL_LINES };

    VertexBufferBase(UsageType usage_type = DYNAMIC)
        : m_usage_type{usage_type}
        , m_vbo{0}
        , m_ebo{0}
        , m_vao{0}
        , m_vertex_count{0}
        , m_indices_count{0}
        , m_valid{false}
    {}
    virtual ~VertexBufferBase() { destroy(); }

    UsageType getUsageType() const { return m_usage_type; }
    int32_t getVertexCount() const { return m_vertex_count; }
    int32_t getIndicesCount() const { return m_indices_count; }
    uint32_t getVboId() const { return m_vbo; };
    uint32_t getEboId() const { return m_ebo; };
    uint32_t getVaoId() const { return m_vao; };
    bool isValid() const { return m_valid; }

    void destroy()
    {
        if (!m_valid)
            return;

        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);
        glDeleteVertexArrays(1, &m_vao);

        m_vbo = 0;
        m_ebo = 0;
        m_vao = 0;
        m_vertex_count = 0;
        m_indices_count = 0;
        m_bounds = {glm::vec3{0.0f}, glm::vec3{0.0f}};
        m_valid = false;
    }

    void draw(PrimitiveType primitive_type = TRIANGLES) const
    {
        if (m_valid) {
            bind(this);
            glDrawElements(primitive_type, m_indices_count, GL_UNSIGNED_INT, 0);
            bind(nullptr);
        }
    }

    static void bind(const VertexBufferBase *vertex_buffer)
    {
        if (vertex_buffer == nullptr) {
            glBindVertexArray(0);
            return;
        }

        if (vertex_buffer->m_valid)
            glBindVertexArray(vertex_buffer->m_vao);
    }

protected:
    UsageType m_usage_type;
    uint32_t m_vbo;
    uint32_t m_ebo;
    uint32_t m_vao;
    int32_t m_vertex_count;
    int32_t m_indices_count;
    std::pair<glm::vec3, glm::vec3> m_bounds;
    bool m_valid;
};

template<typename T, int32_t... Is>
class VertexBuffer : public VertexBufferBase
{
public:
    VertexBuffer(UsageType usage_type = STATIC)
        : VertexBufferBase{usage_type}
    {}
    VertexBuffer(const std::vector<T> &vertices, UsageType usage_type = STATIC)
        : VertexBufferBase{usage_type}
    {
        create(vertices);
    }
    ~VertexBuffer() = default;

    void create(int32_t vertex_count, int32_t indices_count)
    {
        destroy();

        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);

        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * vertex_count, nullptr, m_usage_type);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(uint32_t) * indices_count,
                     nullptr,
                     m_usage_type);

        std::array<int32_t, ATTRS_COUNT> attrs = {POSITION_SIZE, Is...};
        int32_t byte_offset = 0;
        for (int32_t i = 0; i < ATTRS_COUNT; ++i) {
            glVertexAttribPointer(i,
                                  attrs[i],
                                  GL_FLOAT,
                                  GL_FALSE,
                                  sizeof(T),
                                  (GLvoid *) (byte_offset * sizeof(GLfloat)));
            glEnableVertexAttribArray(i);
            byte_offset += attrs[i];
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        m_vertex_count = vertex_count;
        m_indices_count = indices_count;
        m_valid = true;
    }

    void update(const std::vector<T> &vertices, const std::vector<uint32_t> &indices)
    {
        if (!m_valid)
            return;

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        if (vertices.size() > m_vertex_count) {
            glBufferData(GL_ARRAY_BUFFER, sizeof(T) * vertices.size(), 0, m_usage_type);
            m_vertex_count = vertices.size();
        }

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(T) * vertices.size(), vertices.data());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

        if (indices.size() > m_indices_count) {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         sizeof(uint32_t) * indices.size(),
                         0,
                         m_usage_type);
            m_indices_count = indices.size();
        }

        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                        0,
                        sizeof(uint32_t) * indices.size(),
                        indices.data());

        for (const auto &vertex : vertices) {
            m_bounds.first.x = std::min(vertex.position.x, m_bounds.first.x);
            m_bounds.first.y = std::min(vertex.position.y, m_bounds.first.y);
            m_bounds.first.z = std::min(vertex.position.z, m_bounds.first.z);

            m_bounds.second.x = std::max(vertex.position.x, m_bounds.second.x);
            m_bounds.second.y = std::max(vertex.position.y, m_bounds.second.y);
            m_bounds.second.z = std::max(vertex.position.z, m_bounds.second.z);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

private:
    static constexpr int32_t ATTRS_COUNT = sizeof...(Is) + 1;
    static const int32_t POSITION_SIZE = 3;
};

using DefaultVertexBuffer = VertexBuffer<Vertex, 4, 2>;

} // namespace eb

#endif // EB_GRAPHICS_VERTEXBUFFER_H
