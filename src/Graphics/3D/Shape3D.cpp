#include "Shape3D.h"

namespace eb {

vec3 computeFaceNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
    auto a = p3 - p2;
    auto b = p1 - p2;
    return glm::normalize(glm::cross(a, b));
}

Shape3D::Shape3D()
    : VertexArrayInstance{}
{}

void Shape3D::createCube(const vec3 &from, const vec3 &to)
{
    std::vector<Vertex3D> vertices;
    std::vector<uint32_t> indices;

    // back
    int32_t vertices_offset = vertices.size();
    vertices.push_back(
        Vertex3D{{to.x, from.y, from.z}, vec3{0.0f, 0.0f, -1.0f}, vec4{1.0f}, {0.0f, 1.0f}});
    vertices.push_back(Vertex3D{from, vec3{0.0f, 0.0f, -1.0f}, vec4{1.0f}, {1.0f, 1.0f}});
    vertices.push_back(
        Vertex3D{{from.x, to.y, from.z}, vec3{0.0f, 0.0f, -1.0f}, vec4{1.0f}, {1.0f, 0.0f}});
    vertices.push_back(
        Vertex3D{{to.x, to.y, from.z}, vec3{0.0f, 0.0f, -1.0f}, vec4{1.0f}, {0.0f, 0.0f}});

    indices.push_back(vertices_offset);
    indices.push_back(vertices_offset + 1);
    indices.push_back(vertices_offset + 3);
    indices.push_back(vertices_offset + 1);
    indices.push_back(vertices_offset + 2);
    indices.push_back(vertices_offset + 3);

    // front
    vertices_offset = vertices.size();
    vertices.push_back(
        Vertex3D{{from.x, from.y, to.z}, vec3{0.0f, 0.0f, 1.0f}, vec4{1.0f}, {0.0f, 1.0f}});
    vertices.push_back(
        Vertex3D{{to.x, from.y, to.z}, vec3{0.0f, 0.0f, 1.0f}, vec4{1.0f}, {1.0f, 1.0f}});
    vertices.push_back(
        Vertex3D{{to.x, to.y, to.z}, vec3{0.0f, 0.0f, 1.0f}, vec4{1.0f}, {1.0f, 0.0f}});
    vertices.push_back(
        Vertex3D{{from.x, to.y, to.z}, vec3{0.0f, 0.0f, 1.0f}, vec4{1.0f}, {0.0f, 0.0f}});

    indices.push_back(vertices_offset);
    indices.push_back(vertices_offset + 1);
    indices.push_back(vertices_offset + 3);
    indices.push_back(vertices_offset + 1);
    indices.push_back(vertices_offset + 2);
    indices.push_back(vertices_offset + 3);

    // up
    vertices_offset = vertices.size();
    vertices.push_back(
        Vertex3D{{from.x, to.y, to.z}, vec3{0.0f, 1.0f, 0.0f}, vec4{1.0f}, {0.0f, 1.0f}});
    vertices.push_back(
        Vertex3D{{to.x, to.y, to.z}, vec3{0.0f, 1.0f, 0.0f}, vec4{1.0f}, {1.0f, 1.0f}});
    vertices.push_back(
        Vertex3D{{to.x, to.y, from.z}, vec3{0.0f, 1.0f, 0.0f}, vec4{1.0f}, {1.0f, 0.0f}});
    vertices.push_back(
        Vertex3D{{from.x, to.y, from.z}, vec3{0.0f, 1.0f, 0.0f}, vec4{1.0f}, {0.0f, 0.0f}});

    indices.push_back(vertices_offset);
    indices.push_back(vertices_offset + 1);
    indices.push_back(vertices_offset + 3);
    indices.push_back(vertices_offset + 1);
    indices.push_back(vertices_offset + 2);
    indices.push_back(vertices_offset + 3);

    // down
    vertices_offset = vertices.size();
    vertices.push_back(
        Vertex3D{{to.x, from.y, to.z}, vec3{0.0f, -1.0f, 0.0f}, vec4{1.0f}, {0.0f, 1.0f}});
    vertices.push_back(
        Vertex3D{{from.x, from.y, to.z}, vec3{0.0f, -1.0f, 0.0f}, vec4{1.0f}, {1.0f, 1.0f}});
    vertices.push_back(
        Vertex3D{{from.x, from.y, from.z}, vec3{0.0f, -1.0f, 0.0f}, vec4{1.0f}, {1.0f, 0.0f}});
    vertices.push_back(
        Vertex3D{{to.x, from.y, from.z}, vec3{0.0f, -1.0f, 0.0f}, vec4{1.0f}, {0.0f, 0.0f}});

    indices.push_back(vertices_offset);
    indices.push_back(vertices_offset + 1);
    indices.push_back(vertices_offset + 3);
    indices.push_back(vertices_offset + 1);
    indices.push_back(vertices_offset + 2);
    indices.push_back(vertices_offset + 3);

    // left
    vertices_offset = vertices.size();
    vertices.push_back(
        Vertex3D{{from.x, from.y, from.z}, vec3{-1.0f, 0.0f, 0.0f}, vec4{1.0f}, {0.0f, 1.0f}});
    vertices.push_back(
        Vertex3D{{from.x, from.y, to.z}, vec3{-1.0f, 0.0f, 0.0f}, vec4{1.0f}, {1.0f, 1.0f}});
    vertices.push_back(
        Vertex3D{{from.x, to.y, to.z}, vec3{-1.0f, 0.0f, 0.0f}, vec4{1.0f}, {1.0f, 0.0f}});
    vertices.push_back(
        Vertex3D{{from.x, to.y, from.z}, vec3{-1.0f, 0.0f, 0.0f}, vec4{1.0f}, {0.0f, 0.0f}});

    indices.push_back(vertices_offset);
    indices.push_back(vertices_offset + 1);
    indices.push_back(vertices_offset + 3);
    indices.push_back(vertices_offset + 1);
    indices.push_back(vertices_offset + 2);
    indices.push_back(vertices_offset + 3);

    // right
    vertices_offset = vertices.size();
    vertices.push_back(
        Vertex3D{{to.x, from.y, to.z}, vec3{1.0f, 0.0f, 0.0f}, vec4{1.0f}, {0.0f, 1.0f}});
    vertices.push_back(
        Vertex3D{{to.x, from.y, from.z}, vec3{1.0f, 0.0f, 0.0f}, vec4{1.0f}, {1.0f, 1.0f}});
    vertices.push_back(
        Vertex3D{{to.x, to.y, from.z}, vec3{1.0f, 0.0f, 0.0f}, vec4{1.0f}, {1.0f, 0.0f}});
    vertices.push_back(
        Vertex3D{{to.x, to.y, to.z}, vec3{1.0f, 0.0f, 0.0f}, vec4{1.0f}, {0.0f, 0.0f}});

    indices.push_back(vertices_offset);
    indices.push_back(vertices_offset + 1);
    indices.push_back(vertices_offset + 3);
    indices.push_back(vertices_offset + 1);
    indices.push_back(vertices_offset + 2);
    indices.push_back(vertices_offset + 3);

    create(vertices, indices);
}

void Shape3D::createPlane(const vec3 &p1, const vec3 &p2, const vec3 &p3, const vec3 &p4)
{
    std::vector<Vertex3D> vertices;
    std::vector<uint32_t> indices;

    vertices.push_back(Vertex3D{p1, computeFaceNormal(p1, p2, p4), vec4{1.0f}, {0.0f, 1.0f}});
    vertices.push_back(Vertex3D{p2, computeFaceNormal(p2, p4, p1), vec4{1.0f}, {1.0f, 1.0f}});
    vertices.push_back(Vertex3D{p3, computeFaceNormal(p3, p4, p2), vec4{1.0f}, {1.0f, 0.0f}});
    vertices.push_back(Vertex3D{p4, computeFaceNormal(p4, p2, p3), vec4{1.0f}, {0.0f, 0.0f}});

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(3);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(3);

    create(vertices, indices);
}

} // namespace eb
