#ifndef EB_GRAPHICS_VERTEXARRAYINSTANCE_H
#define EB_GRAPHICS_VERTEXARRAYINSTANCE_H

#include "../Common/Drawable.h"
#include "../Common/Enums.h"
#include "../Common/VertexArray.h"
#include "Material.h"
#include "Vertex3D.h"

namespace eb {

class VertexArrayInstance : public Drawable
{
public:
    VertexArrayInstance();
    VertexArrayInstance(const std::vector<Vertex3D> &vertices,
                        const std::vector<uint32_t> &indices,
                        const Material &material);
    ~VertexArrayInstance() = default;

    PrimitiveType getPrimitiveType() const;
    void setPrimitiveType(PrimitiveType primitive_type);

    const std::vector<Vertex3D> &getVertices() const;
    const std::vector<uint32_t> &getIndices() const;

    const Material &getMaterial() const;
    void setMaterial(const Material &material);

    void create(const std::vector<Vertex3D> &vertices, const std::vector<uint32_t> &indices);
    void create(const std::vector<Vertex3D> &vertices,
                const std::vector<uint32_t> &indices,
                const Material &material);
    void destroy();

    void draw(const RenderState &render_state) const;

private:
    PrimitiveType m_primitive_type;
    std::vector<Vertex3D> m_vertices;
    std::vector<uint32_t> m_indices;
    Material m_material;
    VertexArray m_vertex_array;
};

} // namespace eb

#endif // EB_GRAPHICS_VERTEXARRAYINSTANCE_H
