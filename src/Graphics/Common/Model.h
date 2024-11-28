#ifndef EB_GRAPHICS_MODEL_H
#define EB_GRAPHICS_MODEL_H

#include "Mesh.h"

#include <glm/glm.hpp>

#include <vector>

using namespace glm;

namespace eb {

class Model
{
public:
    Model();
    Model(const std::vector<std::shared_ptr<Mesh>> &meshes);
    ~Model();

    void create(const std::vector<std::shared_ptr<Mesh>> &meshes);
    void destroy();

    std::shared_ptr<Mesh> getMeshByName(const std::string &name) const;
    const std::vector<std::shared_ptr<Mesh>> &getMeshes() const;

private:
    std::vector<std::shared_ptr<Mesh>> m_meshes;
};

} // namespace eb

#endif // EB_GRAPHICS_MODEL_H
