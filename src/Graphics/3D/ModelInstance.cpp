#include "ModelInstance.h"

using namespace glm;

namespace eb {

// class AssimpGLMHelpers
// {
// public:
//     static inline mat4 convertMatrixToGLMFormat(const aiMatrix4x4 &from)
//     {
//         glm::mat4 to;
//         to[0][0] = from.a1;
//         to[1][0] = from.a2;
//         to[2][0] = from.a3;
//         to[3][0] = from.a4;
//         to[0][1] = from.b1;
//         to[1][1] = from.b2;
//         to[2][1] = from.b3;
//         to[3][1] = from.b4;
//         to[0][2] = from.c1;
//         to[1][2] = from.c2;
//         to[2][2] = from.c3;
//         to[3][2] = from.c4;
//         to[0][3] = from.d1;
//         to[1][3] = from.d2;
//         to[2][3] = from.d3;
//         to[3][3] = from.d4;
//         return to;
//     }

//     static inline glm::vec3 getGLMVec(const aiVector3D &vec)
//     {
//         return glm::vec3(vec.x, vec.y, vec.z);
//     }

//     static inline quat getGLMQuat(const aiQuaternion &pOrientation)
//     {
//         return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
//     }
// };

ModelInstance::ModelInstance()
    : Drawable{}
{}

ModelInstance::ModelInstance(const std::shared_ptr<Model> &model)
    : Drawable{}
{
    setModel(model);
}

std::shared_ptr<Model> ModelInstance::getModel() const
{
    return m_model;
}

void ModelInstance::setModel(const std::shared_ptr<Model> &model)
{
    m_model = model;

    if (!m_model)
        return;

    m_meshes.clear();

    for (auto const &mesh : model->getMeshes()) {
        auto mesh_instance = std::make_shared<MeshInstance>(mesh);
        m_meshes.emplace(mesh->getName(), mesh_instance);
    }
}

std::shared_ptr<MeshInstance> ModelInstance::getMeshByName(const std::string &name) const
{
    auto found = m_meshes.find(name);
    if (found == m_meshes.end())
        return nullptr;
    return found->second;
}

void ModelInstance::draw(const RenderState &render_state) const
{
    if (!m_meshes.empty()) {
        RenderState n_render_state = render_state;
        n_render_state.model_transform *= getTransform();

        for (const auto &mesh : m_meshes)
            mesh.second->draw(n_render_state);
    }
}

} // namespace eb
