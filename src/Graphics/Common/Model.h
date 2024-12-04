#ifndef EB_GRAPHICS_MODEL_H
#define EB_GRAPHICS_MODEL_H

#include "Mesh.h"
#include "Texture.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>

#include <filesystem>
#include <vector>

using namespace glm;

namespace eb {

class Model
{
public:
    using Meshes = std::vector<std::shared_ptr<Mesh>>;
    using Textures = std::vector<std::pair<std::string, std::shared_ptr<Texture>>>;

    Model();
    Model(const Meshes &meshes, const Textures &textures);
    ~Model();

    bool loadFromFile(const std::filesystem::path &path);

    void create(const Meshes &meshes, const Textures &textures);
    void destroy();

    std::shared_ptr<Mesh> getMeshByName(const std::string &name) const;
    const Meshes &getMeshes() const;
    const Textures &getTextures() const;

private:
    void processNode(aiNode *ai_node, const aiScene *ai_scene, const std::filesystem::path &path);
    void processMesh(aiMesh *ai_mesh, const aiScene *ai_scene, const std::filesystem::path &path);
    std::pair<std::string, std::shared_ptr<Texture>> loadMaterialTextures(
        aiMaterial *ai_material,
        const aiScene *ai_scene,
        aiTextureType type,
        const std::filesystem::path &path);

private:
    Meshes m_meshes;
    Textures m_textures;
};

} // namespace eb

#endif // EB_GRAPHICS_MODEL_H
