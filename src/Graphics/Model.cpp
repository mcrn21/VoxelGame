#include "Model.h"
#include "DefaultShaders.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <spdlog/spdlog.h>

#include <map>

namespace eb {

struct ModelLoader
{
    ModelLoader(Model *model)
        : model{model}
    {}

    bool loadFromFile(const std::filesystem::path &path)
    {
        model->m_meshes.clear();
        model->m_textures.clear();

        auto a_path = std::filesystem::absolute(path);

        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(a_path.string(),
                                                 aiProcess_Triangulate | aiProcess_GenSmoothNormals
                                                     | aiProcess_FlipUVs
                                                     | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            spdlog::error("Assimp: {}", importer.GetErrorString());
            return false;
        }

        processNode(scene->mRootNode, scene, a_path);

        return true;
    }

    void processNode(aiNode *node, const aiScene *scene, const std::filesystem::path &path)
    {
        for (int32_t i = 0; i < node->mNumMeshes; ++i) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            model->m_meshes.push_back(processMesh(mesh, scene, path));
        }

        for (int32_t i = 0; i < node->mNumChildren; i++)
            processNode(node->mChildren[i], scene, path);
    }

    std::shared_ptr<Mesh> processMesh(aiMesh *mesh,
                                      const aiScene *scene,
                                      const std::filesystem::path &path)
    {
        std::vector<MeshVertex> vertices;
        std::vector<uint32_t> indices;

        for (int32_t i = 0; i < mesh->mNumVertices; ++i) {
            MeshVertex vertex;
            glm::vec3 vector;

            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;

            if (mesh->HasNormals()) {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.normal = vector;
            }

            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.tex_coords = vec;

                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.tangent = vector;

                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.bitangent = vector;
            } else
                vertex.tex_coords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }

        for (int32_t i = 0; i < mesh->mNumFaces; ++i) {
            aiFace face = mesh->mFaces[i];
            for (int32_t j = 0; j < face.mNumIndices; ++j)
                indices.push_back(face.mIndices[j]);
        }

        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<std::shared_ptr<::eb::Texture>> textures;

        auto diffuse_textures = loadMaterialTextures(material, aiTextureType_DIFFUSE, path);
        textures.insert(textures.end(), diffuse_textures.begin(), diffuse_textures.end());

        auto specular_textures = loadMaterialTextures(material, aiTextureType_SPECULAR, path);
        textures.insert(textures.end(), specular_textures.begin(), specular_textures.end());

        auto height_textures = loadMaterialTextures(material, aiTextureType_HEIGHT, path);
        textures.insert(textures.end(), height_textures.begin(), height_textures.end());

        auto ambient_textures = loadMaterialTextures(material, aiTextureType_AMBIENT, path);
        textures.insert(textures.end(), ambient_textures.begin(), ambient_textures.end());

        return std::make_shared<Mesh>(vertices, indices, textures);
    }

    std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial *mat,
                                                               aiTextureType type,
                                                               const std::filesystem::path &path)
    {
        auto parent_path = path.parent_path();
        std::vector<std::shared_ptr<Texture>> textures;

        for (int32_t i = 0; i < mat->GetTextureCount(type); ++i) {
            aiString str;
            mat->GetTexture(type, i, &str);
            std::string texture_path{str.data, str.length};
            std::replace(texture_path.begin(), texture_path.end(), '\\', '/');

            auto found = loaded_textures.find(texture_path);
            if (found != loaded_textures.end()) {
                textures.push_back(found->second);
            } else {
                auto texture = std::make_shared<::eb::Texture>();

                std::filesystem::path a_texture_path{parent_path.string() + "/" + texture_path};
                if (!texture->loadFromFile(a_texture_path)) {
                    spdlog::error("Texture not loaded: {}", a_texture_path.string());
                } else {
                    if (type == aiTextureType_DIFFUSE) {
                        texture->setType(::eb::Texture::DIFFUSE);
                    } else if (type == aiTextureType_SPECULAR) {
                        texture->setType(::eb::Texture::SPECULAR);
                    } else if (type == aiTextureType_HEIGHT) {
                        texture->setType(::eb::Texture::HEIGHT);
                    } else if (type == aiTextureType_AMBIENT) {
                        texture->setType(::eb::Texture::AMBIENT);
                    }

                    loaded_textures.emplace(texture_path, texture);
                    textures.push_back(texture);
                }
            }
        }

        return textures;
    }

    Model *model;
    std::map<std::string, std::shared_ptr<Texture>> loaded_textures;
};

Model::Model()
    : Drawable{}
    , Transformable{}
{}

bool Model::loadFromFile(const std::filesystem::path &path)
{
    ModelLoader loader{this};
    return loader.loadFromFile(path);
}

void Model::draw(const RenderTarget &render_target, const RenderState &render_state) const
{
    RenderState new_render_state = render_state;
    new_render_state.transform *= getTransform();
    for (const auto &mesh : m_meshes)
        mesh->draw(render_target, new_render_state);
}

} // namespace eb
