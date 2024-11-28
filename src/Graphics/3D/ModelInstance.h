#ifndef EB_GRAPHICS_3D_MODELINSTANCE_H
#define EB_GRAPHICS_3D_MODELINSTANCE_H

#include "../Common/Drawable.h"
#include "../Common/Model.h"
#include "MeshInstance.h"

#include <unordered_map>

namespace eb {

class ModelInstance : public Drawable
{
public:
    ModelInstance();
    ModelInstance(const std::shared_ptr<Model> &model);
    ~ModelInstance() = default;

    std::shared_ptr<Model> getModel() const;
    void setModel(const std::shared_ptr<Model> &model);

    std::shared_ptr<MeshInstance> getMeshByName(const std::string &name) const;

    void draw(const RenderState &render_state) const;

private:
    std::shared_ptr<Model> m_model;
    std::unordered_map<std::string, std::shared_ptr<MeshInstance>> m_meshes;
};

} // namespace eb

#endif // EB_GRAPHICS_3D_MODELINSTANCE_H
