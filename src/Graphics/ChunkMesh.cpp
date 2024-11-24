#include "ChunkMesh.h"
#include "../Utils/VecUtils.h"
#include "../Voxel/Chunk.h"
#include "../Voxel/Chunks.h"
#include "DefaultShaders.h"

namespace eb {

static const glm::i32vec3 NEIGHBOURS[6]
    = {{0, 0, 1}, {0, 0, -1}, {0, 1, 0}, {0, -1, 0}, {1, 0, 0}, {-1, 0, 0}};

static const glm::i32vec3 FRONT_SIDE_NEIGHBOURS[9] = {{0, 0, 1},
                                                      {-1, 1, 1},
                                                      {0, 1, 1},
                                                      {1, 1, 1},
                                                      {1, 0, 1},
                                                      {1, -1, 1},
                                                      {0, -1, 1},
                                                      {-1, -1, 1},
                                                      {-1, 0, 1}};

static const glm::i32vec3 BACK_SIDE_NEIGHBOURS[9] = {{0, 0, -1},
                                                     {1, 1, -1},
                                                     {0, 1, -1},
                                                     {-1, 1, -1},
                                                     {-1, 0, -1},
                                                     {-1, -1, -1},
                                                     {0, -1, -1},
                                                     {1, -1, -1},
                                                     {1, 0, -1}};

static const glm::i32vec3 UP_SIDE_NEIGHBOURS[9] = {{0, 1, 0},
                                                   {-1, 1, -1},
                                                   {0, 1, -1},
                                                   {1, 1, -1},
                                                   {1, 1, 0},
                                                   {1, 1, 1},
                                                   {0, 1, 1},
                                                   {-1, 1, 1},
                                                   {-1, 1, 0}};

static const glm::i32vec3 DOWN_SIDE_NEIGHBOURS[9] = {{0, -1, 0},
                                                     {1, -1, -1},
                                                     {0, -1, -1},
                                                     {-1, -1, -1},
                                                     {-1, -1, 0},
                                                     {-1, -1, 1},
                                                     {0, -1, 1},
                                                     {1, -1, 1},
                                                     {1, -1, 0}};

static const glm::i32vec3 RIGHT_SIDE_NEIGHBOURS[9] = {{1, 0, 0},
                                                      {1, 1, 1},
                                                      {1, 1, 0},
                                                      {1, 1, -1},
                                                      {1, 0, -1},
                                                      {1, -1, -1},
                                                      {1, -1, 0},
                                                      {1, -1, 1},
                                                      {1, 0, 1}};

static const glm::i32vec3 LEFT_SIDE_NEIGHBOURS[9] = {{-1, 0, 0},
                                                     {-1, 1, -1},
                                                     {-1, 1, 0},
                                                     {-1, 1, 1},
                                                     {-1, 0, 1},
                                                     {-1, -1, 1},
                                                     {-1, -1, 0},
                                                     {-1, -1, -1},
                                                     {-1, 0, -1}};

void ChunkMesh::Light::calculate(Chunks *chunks,
                                 const glm::i32vec3 &voxel_coords,
                                 const glm::i32vec3 (&neighbours)[9])
{
    for (int32_t i = 0; i < 4; ++i) {
        float l = chunks->getLight(voxel_coords + neighbours[0], i);

        float a = chunks->getLight(voxel_coords + neighbours[1], i);
        float b = chunks->getLight(voxel_coords + neighbours[2], i);
        float c = chunks->getLight(voxel_coords + neighbours[3], i);
        float d = chunks->getLight(voxel_coords + neighbours[4], i);

        float e = chunks->getLight(voxel_coords + neighbours[5], i);
        float f = chunks->getLight(voxel_coords + neighbours[6], i);
        float g = chunks->getLight(voxel_coords + neighbours[7], i);
        float h = chunks->getLight(voxel_coords + neighbours[8], i);

        ((reinterpret_cast<float *>(&p1))[i]) = (l * f0 + h + g + f) / f1;
        ((reinterpret_cast<float *>(&p2))[i]) = (l * f0 + f + e + d) / f1;
        ((reinterpret_cast<float *>(&p3))[i]) = (l * f0 + d + c + b) / f1;
        ((reinterpret_cast<float *>(&p4))[i]) = (l * f0 + b + a + h) / f1;
    }
}

ChunkMesh::ChunkMesh(Engine *engine)
    : EngineObject{engine}
    , Drawable{}
    , Transformable{}
{
    m_vertex_array.create<VoxelVertex, 3, 2, 4>();
}

ChunkMesh::ChunkMesh(Engine *engine, const std::shared_ptr<Texture> &texture)
    : EngineObject{engine}
    , Drawable{}
    , Transformable{}
{
    m_vertex_array.create<VoxelVertex, 3, 2, 4>();
    setTexture(texture);
}

std::shared_ptr<Texture> ChunkMesh::getTexture() const
{
    return m_texture;
}

void ChunkMesh::setTexture(const std::shared_ptr<Texture> &texture)
{
    m_texture = texture;
}

void ChunkMesh::create(Chunks *chunks, const glm::i32vec3 &chunk_coords)
{
    float voxel_size = chunks->getVoxelSize();
    float texture_size = chunks->getTextureSize();

    Light light;
    std::vector<VoxelVertex> vertices;
    std::vector<uint32_t> indices;

    chunks->forEachVoxelsInChunk(
        chunk_coords,
        [this, &chunks, &voxel_size, &texture_size, &light, &vertices, &indices](
            const glm::i32vec3 &voxel_coords_in_chunk, const glm::i32vec3 &voxel_coords) {
            auto *voxel = chunks->getVoxel(voxel_coords);

            if (voxel->id == 0)
                return;

            auto uv = m_texture->getUVRect(
                {texture_size * (voxel->id - 1), 0, texture_size, texture_size});

            glm::vec3 offset = static_cast<glm::vec3>(voxel_coords_in_chunk) * voxel_size;

            // Z+ Front side
            if (!chunks->isVoxelBlocked(voxel_coords + FRONT_SIDE_NEIGHBOURS[0])) {
                light.calculate(chunks, voxel_coords, FRONT_SIDE_NEIGHBOURS);
                createFrontSide(vertices, indices, offset, uv, voxel_size, light);
            }

            // Z- Back side
            if (!chunks->isVoxelBlocked(voxel_coords + BACK_SIDE_NEIGHBOURS[0])) {
                light.calculate(chunks, voxel_coords, BACK_SIDE_NEIGHBOURS);
                createBackSide(vertices, indices, offset, uv, voxel_size, light);
            }

            // Y+ Up side
            if (!chunks->isVoxelBlocked(voxel_coords + UP_SIDE_NEIGHBOURS[0])) {
                light.calculate(chunks, voxel_coords, UP_SIDE_NEIGHBOURS);
                createUpSide(vertices, indices, offset, uv, voxel_size, light);
            }

            // Y- Down side
            if (!chunks->isVoxelBlocked(voxel_coords + DOWN_SIDE_NEIGHBOURS[0])) {
                light.calculate(chunks, voxel_coords, DOWN_SIDE_NEIGHBOURS);
                createDownSide(vertices, indices, offset, uv, voxel_size, light);
            }

            // X+ Right side
            if (!chunks->isVoxelBlocked(voxel_coords + RIGHT_SIDE_NEIGHBOURS[0])) {
                light.calculate(chunks, voxel_coords, RIGHT_SIDE_NEIGHBOURS);
                createRightSide(vertices, indices, offset, uv, voxel_size, light);
            }

            // X- Left side
            if (!chunks->isVoxelBlocked(voxel_coords + LEFT_SIDE_NEIGHBOURS[0])) {
                light.calculate(chunks, voxel_coords, LEFT_SIDE_NEIGHBOURS);
                createLeftSide(vertices, indices, offset, uv, voxel_size, light);
            }
        });

    m_vertex_array.setData(vertices, indices);
}

void ChunkMesh::draw(const RenderTarget &render_target, const RenderState &render_state) const
{
    RenderState new_render_state = render_state;
    new_render_state.transform *= getTransform();
    new_render_state.shader = DefaultShaders::getVoxels().get();
    new_render_state.texture = m_texture.get();
    render_target.draw3D(m_vertex_array, new_render_state);
}

void ChunkMesh::createUpSide(std::vector<VoxelVertex> &vertices,
                             std::vector<uint32_t> &indices,
                             const glm::vec3 &offset,
                             const glm::vec4 &uv,
                             float voxel_size,
                             const Light &light)
{
    appendIndices(indices, vertices.size());

    vertices.push_back(VoxelVertex{{offset.x, offset.y + voxel_size, offset.z + voxel_size},
                                   {uv.x, uv.w},
                                   light.p1});
    vertices.push_back(
        VoxelVertex{{offset.x + voxel_size, offset.y + voxel_size, offset.z + voxel_size},
                    {uv.z, uv.w},
                    light.p2});
    vertices.push_back(VoxelVertex{{offset.x + voxel_size, offset.y + voxel_size, offset.z},
                                   {uv.z, uv.y},
                                   light.p3});
    vertices.push_back(
        VoxelVertex{{offset.x, offset.y + voxel_size, offset.z}, {uv.x, uv.y}, light.p4});
}

void ChunkMesh::createDownSide(std::vector<VoxelVertex> &vertices,
                               std::vector<uint32_t> &indices,
                               const glm::vec3 &offset,
                               const glm::vec4 &uv,
                               float voxel_size,
                               const Light &light)
{
    appendIndices(indices, vertices.size());

    vertices.push_back(VoxelVertex{{offset.x + voxel_size, offset.y, offset.z + voxel_size},
                                   {uv.x, uv.w},
                                   light.p1});
    vertices.push_back(
        VoxelVertex{{offset.x, offset.y, offset.z + voxel_size}, {uv.z, uv.w}, light.p2});
    vertices.push_back(VoxelVertex{{offset.x, offset.y, offset.z}, {uv.z, uv.y}, light.p3});
    vertices.push_back(
        VoxelVertex{{offset.x + voxel_size, offset.y, offset.z}, {uv.x, uv.y}, light.p4});
}

void ChunkMesh::createLeftSide(std::vector<VoxelVertex> &vertices,
                               std::vector<uint32_t> &indices,
                               const glm::vec3 &offset,
                               const glm::vec4 &uv,
                               float voxel_size,
                               const Light &light)
{
    appendIndices(indices, vertices.size());

    vertices.push_back(VoxelVertex{{offset.x, offset.y, offset.z}, {uv.x, uv.w}, light.p1});
    vertices.push_back(
        VoxelVertex{{offset.x, offset.y, offset.z + voxel_size}, {uv.z, uv.w}, light.p2});
    vertices.push_back(VoxelVertex{{offset.x, offset.y + voxel_size, offset.z + voxel_size},
                                   {uv.z, uv.y},
                                   light.p3});
    vertices.push_back(
        VoxelVertex{{offset.x, offset.y + voxel_size, offset.z}, {uv.x, uv.y}, light.p4});
}

void ChunkMesh::createRightSide(std::vector<VoxelVertex> &vertices,
                                std::vector<uint32_t> &indices,
                                const glm::vec3 &offset,
                                const glm::vec4 &uv,
                                float voxel_size,
                                const Light &light)
{
    appendIndices(indices, vertices.size());

    vertices.push_back(VoxelVertex{{offset.x + voxel_size, offset.y, offset.z + voxel_size},
                                   {uv.x, uv.w},
                                   light.p1});
    vertices.push_back(
        VoxelVertex{{offset.x + voxel_size, offset.y, offset.z}, {uv.z, uv.w}, light.p2});
    vertices.push_back(VoxelVertex{{offset.x + voxel_size, offset.y + voxel_size, offset.z},
                                   {uv.z, uv.y},
                                   light.p3});
    vertices.push_back(
        VoxelVertex{{offset.x + voxel_size, offset.y + voxel_size, offset.z + voxel_size},
                    {uv.x, uv.y},
                    light.p4});
}

void ChunkMesh::createFrontSide(std::vector<VoxelVertex> &vertices,
                                std::vector<uint32_t> &indices,
                                const glm::vec3 &offset,
                                const glm::vec4 &uv,
                                float voxel_size,
                                const Light &light)
{
    appendIndices(indices, vertices.size());

    vertices.push_back(
        VoxelVertex{{offset.x, offset.y, offset.z + voxel_size}, {uv.x, uv.w}, light.p1});
    vertices.push_back(VoxelVertex{{offset.x + voxel_size, offset.y, offset.z + voxel_size},
                                   {uv.z, uv.w},
                                   light.p2});
    vertices.push_back(
        VoxelVertex{{offset.x + voxel_size, offset.y + voxel_size, offset.z + voxel_size},
                    {uv.z, uv.y},
                    light.p3});
    vertices.push_back(VoxelVertex{{offset.x, offset.y + voxel_size, offset.z + voxel_size},
                                   {uv.x, uv.y},
                                   light.p4});
}

void ChunkMesh::createBackSide(std::vector<VoxelVertex> &vertices,
                               std::vector<uint32_t> &indices,
                               const glm::vec3 &offset,
                               const glm::vec4 &uv,
                               float voxel_size,
                               const Light &light)
{
    appendIndices(indices, vertices.size());

    vertices.push_back(
        VoxelVertex{{offset.x + voxel_size, offset.y, offset.z}, {uv.x, uv.w}, light.p1});
    vertices.push_back(VoxelVertex{{offset.x, offset.y, offset.z}, {uv.z, uv.w}, light.p2});
    vertices.push_back(
        VoxelVertex{{offset.x, offset.y + voxel_size, offset.z}, {uv.z, uv.y}, light.p3});
    vertices.push_back(VoxelVertex{{offset.x + voxel_size, offset.y + voxel_size, offset.z},
                                   {uv.x, uv.y},
                                   light.p4});
}

void ChunkMesh::appendIndices(std::vector<uint32_t> &indices, int32_t vertices_offset) const
{
    indices.push_back(vertices_offset);
    indices.push_back(vertices_offset + 1);
    indices.push_back(vertices_offset + 3);
    indices.push_back(vertices_offset + 1);
    indices.push_back(vertices_offset + 2);
    indices.push_back(vertices_offset + 3);
}

} // namespace eb
