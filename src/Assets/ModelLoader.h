#ifndef EB_ASSETS_MODELLOADER_H
#define EB_ASSETS_MODELLOADER_H

#include "../Graphics/Common/Model.h"
#include "AssetLoader.h"
#include "Assets.h"

#include <GL/gl.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <spdlog/spdlog.h>

#include "../3rd/stb_image.h"

#include <filesystem>

namespace eb {

template<>
class AssetLoader<Model>
{
public:
    static std::shared_ptr<Model> loadFromFile(Assets *assets,
                                               const std::string &asset_name,
                                               const std::filesystem::path &path)
    {
        if (path.empty())
            return nullptr;

        std::string name = asset_name.empty() ? path.stem().string() : asset_name;
        if (name.empty())
            return nullptr;

        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path.string(),
                                                 aiProcess_Triangulate | aiProcess_GenSmoothNormals
                                                     | aiProcess_FlipUVs
                                                     | aiProcess_CalcTangentSpace
                                                     | aiProcess_PreTransformVertices);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            spdlog::error("Assimp: {}", importer.GetErrorString());
            return nullptr;
        }

        std::vector<std::shared_ptr<Mesh>> meshes;
        processNode(scene->mRootNode, scene, assets, meshes, name, path);

        auto model = std::make_shared<Model>(meshes);
        assets->add<Model>(name, model);

        return model;
    }

    static void processNode(aiNode *node,
                            const aiScene *scene,
                            Assets *assets,
                            std::vector<std::shared_ptr<Mesh>> &meshes,
                            const std::string &asset_name,
                            const std::filesystem::path &path)
    {
        for (int32_t i = 0; i < node->mNumMeshes; ++i) {
            aiMesh *ai_mesh = scene->mMeshes[node->mMeshes[i]];
            processMesh(ai_mesh, scene, assets, meshes, asset_name, path);
        }

        for (int32_t i = 0; i < node->mNumChildren; i++)
            processNode(node->mChildren[i], scene, assets, meshes, asset_name, path);
    }

    static void processMesh(aiMesh *ai_mesh,
                            const aiScene *scene,
                            Assets *assets,
                            std::vector<std::shared_ptr<Mesh>> &meshes,
                            const std::string &asset_name,
                            const std::filesystem::path &path)
    {
        std::vector<Vertex3D> vertices;
        std::vector<uint32_t> indices;

        for (int32_t i = 0; i < ai_mesh->mNumVertices; ++i) {
            Vertex3D vertex;
            vec3 vector;

            vector.x = ai_mesh->mVertices[i].x;
            vector.y = ai_mesh->mVertices[i].y;
            vector.z = ai_mesh->mVertices[i].z;
            vertex.position = vector;

            if (ai_mesh->HasNormals()) {
                vector.x = ai_mesh->mNormals[i].x;
                vector.y = ai_mesh->mNormals[i].y;
                vector.z = ai_mesh->mNormals[i].z;
                vertex.normal = vector;
            }

            // Texture coordinates
            if (ai_mesh->mTextureCoords[0]) {
                vec2 vec;
                vec.x = ai_mesh->mTextureCoords[0][i].x;
                vec.y = ai_mesh->mTextureCoords[0][i].y;
                vertex.tex_coords = vec;

                // vector.x = mesh->mTangents[i].x;
                // vector.y = mesh->mTangents[i].y;
                // vector.z = mesh->mTangents[i].z;
                // vertex.tangent = vector;

                // vector.x = mesh->mBitangents[i].x;
                // vector.y = mesh->mBitangents[i].y;
                // vector.z = mesh->mBitangents[i].z;
                // vertex.bitangent = vector;
            } else
                vertex.tex_coords = vec2(0.0f, 0.0f);

            // Vertex colors
            if (ai_mesh->HasVertexColors(0)) {
                vec4 color{1.0f};
                color.r = ai_mesh->mColors[0][i].r;
                color.g = ai_mesh->mColors[0][i].g;
                color.b = ai_mesh->mColors[0][i].b;
                color.a = ai_mesh->mColors[0][i].a;

                vertex.color = color;
            } else
                vertex.color = vec4{1.0f};

            vertices.push_back(vertex);
        }

        for (int32_t i = 0; i < ai_mesh->mNumFaces; ++i) {
            aiFace face = ai_mesh->mFaces[i];
            for (int32_t j = 0; j < face.mNumIndices; ++j)
                indices.push_back(face.mIndices[j]);
        }

        aiMaterial *ai_material = scene->mMaterials[ai_mesh->mMaterialIndex];

        Material material;
        material.diffuse_texture0 = loadMaterialTextures(scene,
                                                         ai_material,
                                                         aiTextureType_DIFFUSE,
                                                         assets,
                                                         asset_name,
                                                         path);

        auto mesh = std::make_shared<Mesh>(vertices, indices, material);

        std::string mesh_name{ai_mesh->mName.data, ai_mesh->mName.length};
        int32_t count = std::count_if(meshes.begin(), meshes.end(), [&mesh_name](const auto &m) {
            return m->getName().starts_with(mesh_name);
        });

        if (count > 0)
            mesh_name += "_" + std::to_string(count);

        mesh->setName(mesh_name);

        meshes.push_back(mesh);

        // std::vector<std::shared_ptr<::eb::Texture>> textures;

        // auto diffuse_textures = loadMaterialTextures(ai_material, aiTextureType_DIFFUSE, path);
        // textures.insert(textures.end(), diffuse_textures.begin(), diffuse_textures.end());

        // auto specular_textures = loadMaterialTextures(ai_material, aiTextureType_SPECULAR, path);
        // textures.insert(textures.end(), specular_textures.begin(), specular_textures.end());

        // auto height_textures = loadMaterialTextures(ai_material, aiTextureType_HEIGHT, path);
        // textures.insert(textures.end(), height_textures.begin(), height_textures.end());

        // auto ambient_textures = loadMaterialTextures(ai_material, aiTextureType_AMBIENT, path);
        // textures.insert(textures.end(), ambient_textures.begin(), ambient_textures.end());

        // Material material;
        // if (!diffuse_textures.empty())
        //     material.diffuse_texture0 = diffuse_textures[0];

        // auto mesh = std::make_shared<MeshInstance>(vertices, indices, material);

        // mesh->setName(std::string{ai_mesh->mName.data, ai_mesh->mName.length});

        // return mesh;
    }

    static std::shared_ptr<Texture> loadMaterialTextures(const aiScene *scene,
                                                         aiMaterial *ai_material,
                                                         aiTextureType type,
                                                         Assets *assets,
                                                         const std::string &asset_name,
                                                         const std::filesystem::path &path)
    {
        for (int32_t i = 0; i < ai_material->GetTextureCount(type); ++i) {
            aiString str;
            ai_material->GetTexture(type, i, &str);
            std::string texture_name{str.data, str.length};
            std::string texture_asset_name{asset_name + "_"
                                           + std::filesystem::path{texture_name}.stem().string()};

            std::shared_ptr<Texture> texture;

            if (texture_name.starts_with('*')) {
                const aiTexture *ai_texture = scene->GetEmbeddedTexture(texture_name.data());

                texture = assets->get<Texture>(texture_asset_name);
                if (texture)
                    return texture;

                if (ai_texture) {
                    uint8_t *image_data = nullptr;
                    int32_t width = 0;
                    int32_t height = 0;
                    int32_t nr_components = 0;
                    int32_t data_length = ai_texture->mHeight == 0
                                              ? ai_texture->mWidth
                                              : ai_texture->mWidth * ai_texture->mHeight;

                    image_data = stbi_load_from_memory(reinterpret_cast<uint8_t *>(
                                                           ai_texture->pcData),
                                                       data_length,
                                                       &width,
                                                       &height,
                                                       &nr_components,
                                                       0);

                    if (!image_data)
                        return nullptr;

                    Texture::Format format;
                    if (nr_components == 1)
                        format = Texture::LUMINANCE;
                    else if (nr_components == 2)
                        format = Texture::LUMINANCE_ALPHA;
                    else if (nr_components == 3)
                        format = Texture::RGB;
                    else if (nr_components == 4)
                        format = Texture::RGBA;

                    texture = std::make_shared<Texture>();
                    texture->create({width, height}, format, image_data);
                    assets->add<Texture>(texture_asset_name, texture);

                    stbi_image_free(image_data);

                    return texture;
                }
            }

            std::filesystem::path texture_path{path.parent_path().string() + "/" + texture_name};
            texture = assets->get<Texture>(texture_asset_name);
            if (!texture)
                texture = assets->loadFromFile<Texture>(texture_asset_name, texture_path);
            return texture;
        }

        return nullptr;
    }
};

} // namespace eb

#endif // EB_ASSETS_MODELLOADER_H
