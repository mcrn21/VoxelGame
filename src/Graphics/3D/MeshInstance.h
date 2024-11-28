#ifndef EB_GRAPHICS_3D_MESHINSTANCE_H
#define EB_GRAPHICS_3D_MESHINSTANCE_H

#include "../Common/Drawable.h"
#include "../Common/Mesh.h"

namespace eb {

class MeshInstance : public Drawable
{
public:
    MeshInstance();
    MeshInstance(const std::shared_ptr<Mesh> &mesh);
    ~MeshInstance() = default;

    std::shared_ptr<Mesh> getMesh() const;
    void setMesh(const std::shared_ptr<Mesh> &mesh);

    void draw(const RenderState &render_state) const;

private:
    std::shared_ptr<Mesh> m_mesh;
};

} // namespace eb

#endif // EB_GRAPHICS_3D_MESHINSTANCE_H
