#include "Model.h"
#include "../../Assets/Assets.h"
#include "../../Engine.h"

namespace eb {

Model::Model() {}

Model::Model(const Meshes &meshes, const Textures &textures)
{
    create(meshes, textures);
}

Model::~Model()
{
    destroy();
}

bool Model::loadFromFile(const std::filesystem::path &path)
{
    if (path.empty())
        return false;

    destroy();

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path.string(),
                                             aiProcess_Triangulate | aiProcess_GenSmoothNormals
                                                 | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
                                                 | aiProcess_PreTransformVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        spdlog::error("Assimp: {}", importer.GetErrorString());
        return false;
    }

    processNode(scene->mRootNode, scene, path);

    return true;
}

void Model::create(const Meshes &meshes, const Textures &textures)
{
    m_meshes = meshes;
    m_textures = textures;
}

void Model::destroy()
{
    m_meshes.clear();
    m_textures.clear();
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

const Model::Meshes &Model::getMeshes() const
{
    return m_meshes;
}

const Model::Textures &Model::getTextures() const
{
    return m_textures;
}

void Model::processNode(aiNode *ai_node, const aiScene *ai_scene, const std::filesystem::path &path)
{
    for (int32_t i = 0; i < ai_node->mNumMeshes; ++i) {
        aiMesh *ai_mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
        processMesh(ai_mesh, ai_scene, path);
    }

    for (int32_t i = 0; i < ai_node->mNumChildren; i++)
        processNode(ai_node->mChildren[i], ai_scene, path);
}

void Model::processMesh(aiMesh *ai_mesh, const aiScene *ai_scene, const std::filesystem::path &path)
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

    aiMaterial *ai_material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];

    Material material;
    material.diffuse_texture0
        = loadMaterialTextures(ai_material, ai_scene, aiTextureType_DIFFUSE, path).second;

    auto mesh = std::make_shared<Mesh>(vertices, indices, material);

    std::string mesh_name{ai_mesh->mName.data, ai_mesh->mName.length};
    int32_t count = std::count_if(m_meshes.begin(), m_meshes.end(), [&mesh_name](const auto &m) {
        return m->getName().starts_with(mesh_name);
    });

    if (count > 0)
        mesh_name += "_" + std::to_string(count);

    mesh->setName(mesh_name);

    m_meshes.push_back(mesh);
}

std::pair<std::string, std::shared_ptr<Texture>> Model::loadMaterialTextures(
    aiMaterial *ai_material,
    const aiScene *ai_scene,
    aiTextureType type,
    const std::filesystem::path &path)
{
    for (int32_t i = 0; i < ai_material->GetTextureCount(type); ++i) {
        aiString str;
        ai_material->GetTexture(type, i, &str);
        std::string texture_name{str.data, str.length};
        std::string texture_asset_name{std::filesystem::path{texture_name}.stem().string()};
        std::shared_ptr<Texture> texture;

        if (texture_name.starts_with('*')) {
            const aiTexture *ai_texture = ai_scene->GetEmbeddedTexture(texture_name.data());

            if (ai_texture) {
                texture = std::make_shared<Texture>();
                if (texture->loadFromData(reinterpret_cast<uint8_t *>(ai_texture->pcData),
                                          ai_texture->mHeight == 0
                                              ? ai_texture->mWidth
                                              : ai_texture->mWidth * ai_texture->mHeight))

                    m_textures.emplace_back(std::string{}, texture);
                return {std::string{}, texture};
            }
        }

        std::filesystem::path texture_path{path.parent_path().string() + "/" + texture_name};
        texture = Engine::getInstance()->getAssets().get<Texture>(texture_asset_name);
        if (!texture) {
            texture = std::make_shared<Texture>();
            if (texture->loadFromFile(texture_path)) {
                m_textures.emplace_back(texture_asset_name, texture);
                return {texture_asset_name, texture};
            }
        }

        return {};
    }
}

} // namespace eb
