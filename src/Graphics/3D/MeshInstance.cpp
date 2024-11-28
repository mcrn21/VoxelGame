#include "MeshInstance.h"
#include "../Common/DefaultShaders.h"
#include "../Common/RenderTarget.h"
#include "../Common/Texture.h"

namespace eb {

MeshInstance::MeshInstance()
    : Drawable{}
{}

MeshInstance::MeshInstance(const std::shared_ptr<Mesh> &mesh)
    : Drawable{}
{
    setMesh(mesh);
}

std::shared_ptr<Mesh> MeshInstance::getMesh() const
{
    return m_mesh;
}

void MeshInstance::setMesh(const std::shared_ptr<Mesh> &mesh)
{
    m_mesh = mesh;
}

void MeshInstance::draw(const RenderState &render_state) const
{
    if (m_mesh) {
        render_state.shader->uniformMatrix("u_model", render_state.model_transform * getTransform());
        render_state.shader->uniformMaterial(m_mesh->getMaterial());
        m_mesh->getVertexArray().draw(TRIANGLES);
        Texture::bind(nullptr);
    }
}

} // namespace eb
