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
    enum PrimitiveType { TRIANGLES = GL_TRIANGLES, LINES = GL_LINES };

    VertexBufferBase()
        : m_vbo{}
        , m_vao{}
        , m_size{0}
        , m_valid{false}
    {}
    virtual ~VertexBufferBase() { destroy(); }

    int32_t getSize() const { return m_size; }

    bool isValid() const { return m_valid; }

    void destroy()
    {
        if (!m_valid)
            return;

        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);

        m_vbo = 0;
        m_vao = 0;
        m_size = 0;
        m_bounds = {glm::vec3{0.0f}, glm::vec3{0.0f}};
        m_valid = false;
    }

    void draw(PrimitiveType primitive_type = TRIANGLES) const
    {
        if (m_valid) {
            bind(this);
            glDrawArrays(primitive_type, 0, m_size);
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
    uint32_t m_vbo;
    uint32_t m_vao;
    int32_t m_size;
    std::pair<glm::vec3, glm::vec3> m_bounds;
    bool m_valid;
};

template<typename T, int32_t... Is>
class VertexBuffer : public VertexBufferBase
{
public:
    VertexBuffer()
        : VertexBufferBase{}
    {}
    VertexBuffer(const std::vector<T> &vertices)
        : VertexBufferBase{}
    {
        create(vertices);
    }
    ~VertexBuffer() = default;

    void create(const std::vector<T> &vertices)
    {
        destroy();

        if (vertices.empty())
            return;

        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

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

        glBindVertexArray(0);

        for (const auto &vertex : vertices) {
            m_bounds.first.x = std::min(vertex.position.x, m_bounds.first.x);
            m_bounds.first.y = std::min(vertex.position.y, m_bounds.first.y);
            m_bounds.first.z = std::min(vertex.position.z, m_bounds.first.z);

            m_bounds.second.x = std::max(vertex.position.x, m_bounds.second.x);
            m_bounds.second.y = std::max(vertex.position.y, m_bounds.second.y);
            m_bounds.second.z = std::max(vertex.position.z, m_bounds.second.z);
        }

        m_size = vertices.size();
        m_valid = true;
    }

private:
    static constexpr int32_t ATTRS_COUNT = sizeof...(Is) + 1;
    static const int32_t POSITION_SIZE = 3;
};

using DefaultVertexBuffer = VertexBuffer<Vertex, 4, 2>;

} // namespace eb

#endif // EB_GRAPHICS_VERTEXBUFFER_H
