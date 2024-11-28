#include "Model.h"

namespace eb {

Model::Model() {}

Model::Model(const std::vector<std::shared_ptr<Mesh> > &meshes)
{
    create(meshes);
}

Model::~Model()
{
    destroy();
}

void Model::create(const std::vector<std::shared_ptr<Mesh> > &meshes)
{
    m_meshes = meshes;
}

void Model::destroy()
{
    m_meshes.clear();
}

std::shared_ptr<Mesh> Model::getMeshByName(const std::string &name) const
{
    auto found = std::find_if(m_meshes.begin(), m_meshes.end(), [&name](const auto &m) {
        return m->getName() == name;
    });
    if (found != m_meshes.end())
        return nullptr;
    return *found;
}

const std::vector<std::shared_ptr<Mesh> > &Model::getMeshes() const
{
    return m_meshes;
}

} // namespace eb
