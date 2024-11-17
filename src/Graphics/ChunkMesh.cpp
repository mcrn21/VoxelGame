#include "ChunkMesh.h"
#include "../Voxel/Chunk.h"
#include "../Voxel/Chunks.h"
#include "DefaultShaders.h"

namespace eb {

static glm::i32vec3 NEIGHBOURS[6]
    = {{0, 0, 1}, {0, 0, -1}, {0, 1, 0}, {0, -1, 0}, {1, 0, 0}, {-1, 0, 0}};

ChunkMesh::ChunkMesh(Engine *engine)
    : EngineObject{engine}
    , Drawable{}
    , Transformable{}
{}

ChunkMesh::ChunkMesh(Engine *engine, const std::shared_ptr<Texture> &texture)
    : EngineObject{engine}
    , Drawable{}
    , Transformable{}
{
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

void ChunkMesh::create(Chunk *chunk)
{
    m_vertex_buffer.destroy();

    std::vector<VoxelVertex> vertices;

    float voxel_size = chunk->getVoxelSize();
    float texture_size = chunk->getTextureSize();

    AmbientOcclusion ao;

    chunk->forEach([this,
                    &chunk,
                    &voxel_size,
                    &texture_size,
                    &vertices,
                    &ao](const Voxel *voxel, const glm::i32vec3 &index) {
        if (voxel->id == 0)
            return;

        auto uv = m_texture->getUVRect(
            {texture_size * (voxel->id - 1), 0, texture_size, texture_size});

        glm::vec3 offset = {index.x * voxel_size, index.y * voxel_size, index.z * voxel_size};

        // Z+ Front side
        if (!chunk->isBlocked({index.x, index.y, index.z + 1}))
            createFrontSide(vertices, offset, uv, voxel_size, ao);

        // Z- Back side
        if (!chunk->isBlocked({index.x, index.y, index.z - 1}))
            createBackSide(vertices, offset, uv, voxel_size, ao);

        // Y+ Up side
        if (!chunk->isBlocked({index.x, index.y + 1, index.z}))
            createUpSide(vertices, offset, uv, voxel_size, ao);

        // Y- Down side
        if (!chunk->isBlocked({index.x, index.y - 1, index.z}))
            createDownSide(vertices, offset, uv, voxel_size, ao);

        // X+ Right side
        if (!chunk->isBlocked({index.x + 1, index.y, index.z}))
            createRightSide(vertices, offset, uv, voxel_size, ao);

        // X- Left side
        if (!chunk->isBlocked({index.x - 1, index.y, index.z}))
            createLeftSide(vertices, offset, uv, voxel_size, ao);
    });

    m_vertex_buffer.create(vertices);
}

void ChunkMesh::create(Chunks *chunks, const glm::i32vec3 &index)
{
    m_vertex_buffer.destroy();

    std::vector<VoxelVertex> vertices;

    float voxel_size = chunks->getVoxelSize();
    float texture_size = chunks->getTextureSize();
    glm::i32vec3 voxel_coords_offset = index * chunks->getChunkSize();
    auto *chunk = chunks->getChunk(index);

    AmbientOcclusion ao;

    chunk->forEach([this,
                    &chunks,
                    &chunk,
                    &voxel_size,
                    &texture_size,
                    &voxel_coords_offset,
                    &vertices,
                    &ao](const Voxel *voxel, const glm::i32vec3 &index) {
        if (voxel->id == 0)
            return;

        auto uv = m_texture->getUVRect(
            {texture_size * (voxel->id - 1), 0, texture_size, texture_size});

        glm::vec3 offset = {index.x * voxel_size, index.y * voxel_size, index.z * voxel_size};
        glm::i32vec3 chunks_index = voxel_coords_offset + index;

        ao.clear();

        // Z+ Front side
        if (!chunks->isBlocked(chunks_index + NEIGHBOURS[0])) {
            ao.a = chunks->isBlocked(chunks_index + glm::i32vec3{-1, 1, 1}) * ao.factor;
            ao.b = chunks->isBlocked(chunks_index + glm::i32vec3{0, 1, 1}) * ao.factor;
            ao.c = chunks->isBlocked(chunks_index + glm::i32vec3{1, 1, 1}) * ao.factor;
            ao.d = chunks->isBlocked(chunks_index + glm::i32vec3{1, 0, 1}) * ao.factor;

            ao.e = chunks->isBlocked(chunks_index + glm::i32vec3{1, -1, 1}) * ao.factor;
            ao.f = chunks->isBlocked(chunks_index + glm::i32vec3{0, -1, 1}) * ao.factor;
            ao.g = chunks->isBlocked(chunks_index + glm::i32vec3{-1, -1, 1}) * ao.factor;
            ao.h = chunks->isBlocked(chunks_index + glm::i32vec3{-1, 0, 1}) * ao.factor;

            ao.p1 = ao.light * (1.0f - ao.h - ao.g - ao.f);
            ao.p2 = ao.light * (1.0f - ao.f - ao.e - ao.d);
            ao.p3 = ao.light * (1.0f - ao.b - ao.a - ao.h);

            ao.p4 = ao.light * (1.0f - ao.f - ao.e - ao.d);
            ao.p5 = ao.light * (1.0f - ao.d - ao.c - ao.b);
            ao.p6 = ao.light * (1.0f - ao.b - ao.a - ao.h);

            createFrontSide(vertices, offset, uv, voxel_size, ao);
        }

        ao.clear();

        // Z- Back side
        if (!chunks->isBlocked(chunks_index + NEIGHBOURS[1])) {
            ao.a = chunks->isBlocked(chunks_index + glm::i32vec3{-1, 1, -1}) * ao.factor;
            ao.b = chunks->isBlocked(chunks_index + glm::i32vec3{0, 1, -1}) * ao.factor;
            ao.c = chunks->isBlocked(chunks_index + glm::i32vec3{1, 1, -1}) * ao.factor;
            ao.d = chunks->isBlocked(chunks_index + glm::i32vec3{1, 0, -1}) * ao.factor;

            ao.e = chunks->isBlocked(chunks_index + glm::i32vec3{1, -1, -1}) * ao.factor;
            ao.f = chunks->isBlocked(chunks_index + glm::i32vec3{0, -1, -1}) * ao.factor;
            ao.g = chunks->isBlocked(chunks_index + glm::i32vec3{-1, -1, -1}) * ao.factor;
            ao.h = chunks->isBlocked(chunks_index + glm::i32vec3{-1, 0, -1}) * ao.factor;

            ao.p1 = ao.light * (1.0f - ao.d - ao.e - ao.f);
            ao.p2 = ao.light * (1.0f - ao.h - ao.g - ao.f);
            ao.p3 = ao.light * (1.0f - ao.b - ao.c - ao.d);

            ao.p4 = ao.light * (1.0f - ao.h - ao.g - ao.f);
            ao.p5 = ao.light * (1.0f - ao.h - ao.a - ao.b);
            ao.p6 = ao.light * (1.0f - ao.b - ao.c - ao.d);

            createBackSide(vertices, offset, uv, voxel_size, ao);
        }

        ao.clear();

        // Y+ Up side
        if (!chunks->isBlocked(chunks_index + NEIGHBOURS[2])) {
            ao.a = chunks->isBlocked(chunks_index + glm::i32vec3{-1, 1, -1}) * ao.factor;
            ao.b = chunks->isBlocked(chunks_index + glm::i32vec3{0, 1, -1}) * ao.factor;
            ao.c = chunks->isBlocked(chunks_index + glm::i32vec3{1, 1, -1}) * ao.factor;
            ao.d = chunks->isBlocked(chunks_index + glm::i32vec3{1, 1, 0}) * ao.factor;

            ao.e = chunks->isBlocked(chunks_index + glm::i32vec3{1, 1, 1}) * ao.factor;
            ao.f = chunks->isBlocked(chunks_index + glm::i32vec3{0, 1, 1}) * ao.factor;
            ao.g = chunks->isBlocked(chunks_index + glm::i32vec3{-1, 1, 1}) * ao.factor;
            ao.h = chunks->isBlocked(chunks_index + glm::i32vec3{-1, 1, 0}) * ao.factor;

            ao.p1 = ao.light * (1.0f - ao.h - ao.g - ao.f);
            ao.p2 = ao.light * (1.0f - ao.f - ao.e - ao.d);
            ao.p3 = ao.light * (1.0f - ao.b - ao.a - ao.h);

            ao.p4 = ao.light * (1.0f - ao.f - ao.e - ao.d);
            ao.p5 = ao.light * (1.0f - ao.d - ao.c - ao.b);
            ao.p6 = ao.light * (1.0f - ao.b - ao.a - ao.h);

            createUpSide(vertices, offset, uv, voxel_size, ao);
        }

        ao.clear();

        // Y- Down side
        if (!chunks->isBlocked(chunks_index + NEIGHBOURS[3])) {
            ao.a = chunks->isBlocked(chunks_index + glm::i32vec3{-1, -1, -1}) * ao.factor;
            ao.b = chunks->isBlocked(chunks_index + glm::i32vec3{0, -1, -1}) * ao.factor;
            ao.c = chunks->isBlocked(chunks_index + glm::i32vec3{1, -1, -1}) * ao.factor;
            ao.d = chunks->isBlocked(chunks_index + glm::i32vec3{1, -1, 0}) * ao.factor;

            ao.e = chunks->isBlocked(chunks_index + glm::i32vec3{1, -1, 1}) * ao.factor;
            ao.f = chunks->isBlocked(chunks_index + glm::i32vec3{0, -1, 1}) * ao.factor;
            ao.g = chunks->isBlocked(chunks_index + glm::i32vec3{-1, -1, 1}) * ao.factor;
            ao.h = chunks->isBlocked(chunks_index + glm::i32vec3{-1, -1, 0}) * ao.factor;

            ao.p1 = ao.light * (1.0f - ao.d - ao.e - ao.f);
            ao.p2 = ao.light * (1.0f - ao.h - ao.g - ao.f);
            ao.p3 = ao.light * (1.0f - ao.b - ao.c - ao.d);

            ao.p4 = ao.light * (1.0f - ao.h - ao.g - ao.f);
            ao.p5 = ao.light * (1.0f - ao.h - ao.a - ao.b);
            ao.p6 = ao.light * (1.0f - ao.b - ao.c - ao.d);

            createDownSide(vertices, offset, uv, voxel_size, ao);
        }

        ao.clear();

        // X+ Right side
        if (!chunks->isBlocked(chunks_index + NEIGHBOURS[4])) {
            ao.a = chunks->isBlocked(chunks_index + glm::i32vec3{1, 1, 1}) * ao.factor;
            ao.b = chunks->isBlocked(chunks_index + glm::i32vec3{1, 1, 0}) * ao.factor;
            ao.c = chunks->isBlocked(chunks_index + glm::i32vec3{1, 1, -1}) * ao.factor;
            ao.d = chunks->isBlocked(chunks_index + glm::i32vec3{1, 0, -1}) * ao.factor;

            ao.e = chunks->isBlocked(chunks_index + glm::i32vec3{1, -1, -1}) * ao.factor;
            ao.f = chunks->isBlocked(chunks_index + glm::i32vec3{1, -1, 0}) * ao.factor;
            ao.g = chunks->isBlocked(chunks_index + glm::i32vec3{1, -1, 1}) * ao.factor;
            ao.h = chunks->isBlocked(chunks_index + glm::i32vec3{1, 0, 1}) * ao.factor;

            ao.p1 = ao.light * (1.0f - ao.h - ao.g - ao.f);
            ao.p2 = ao.light * (1.0f - ao.f - ao.e - ao.d);
            ao.p3 = ao.light * (1.0f - ao.b - ao.a - ao.h);

            ao.p4 = ao.light * (1.0f - ao.f - ao.e - ao.d);
            ao.p5 = ao.light * (1.0f - ao.d - ao.c - ao.b);
            ao.p6 = ao.light * (1.0f - ao.b - ao.a - ao.h);

            createRightSide(vertices, offset, uv, voxel_size, ao);
        }

        ao.clear();

        // X- Left side
        if (!chunks->isBlocked(chunks_index + NEIGHBOURS[5])) {
            ao.a = chunks->isBlocked(chunks_index + glm::i32vec3{-1, 1, 1}) * ao.factor;
            ao.b = chunks->isBlocked(chunks_index + glm::i32vec3{-1, 1, 0}) * ao.factor;
            ao.c = chunks->isBlocked(chunks_index + glm::i32vec3{-1, 1, -1}) * ao.factor;
            ao.d = chunks->isBlocked(chunks_index + glm::i32vec3{-1, 0, -1}) * ao.factor;

            ao.e = chunks->isBlocked(chunks_index + glm::i32vec3{-1, -1, -1}) * ao.factor;
            ao.f = chunks->isBlocked(chunks_index + glm::i32vec3{-1, -1, 0}) * ao.factor;
            ao.g = chunks->isBlocked(chunks_index + glm::i32vec3{-1, -1, 1}) * ao.factor;
            ao.h = chunks->isBlocked(chunks_index + glm::i32vec3{-1, 0, 1}) * ao.factor;

            ao.p1 = ao.light * (1.0f - ao.d - ao.e - ao.f);
            ao.p2 = ao.light * (1.0f - ao.h - ao.g - ao.f);
            ao.p3 = ao.light * (1.0f - ao.b - ao.c - ao.d);

            ao.p4 = ao.light * (1.0f - ao.h - ao.g - ao.f);
            ao.p5 = ao.light * (1.0f - ao.h - ao.a - ao.b);
            ao.p6 = ao.light * (1.0f - ao.b - ao.c - ao.d);

            createLeftSide(vertices, offset, uv, voxel_size, ao);
        }
    });

    m_vertex_buffer.create(vertices);
}

void ChunkMesh::draw(const RenderTarget &render_target) const
{
    if (m_texture) {
        RenderState render_state;
        render_state.transform = getTransform();
        render_state.shader = DefaultShaders::getVoxels().get();
        render_state.texture = m_texture.get();
        render_target.draw(m_vertex_buffer, render_state);
    }
}

void ChunkMesh::createUpSide(std::vector<VoxelVertex> &vertices,
                             const glm::vec3 &offset,
                             const glm::vec4 &uv,
                             float voxel_size,
                             const AmbientOcclusion &ao)
{
    vertices.push_back(VoxelVertex{{offset.x, offset.y + voxel_size, offset.z + voxel_size},
                                   glm::vec4{1.0f},
                                   {uv.x, uv.w},
                                   ao.p1});
    vertices.push_back(
        VoxelVertex{{offset.x + voxel_size, offset.y + voxel_size, offset.z + voxel_size},
                    glm::vec4{1.0f},
                    {uv.z, uv.w},
                    ao.p2});
    vertices.push_back(VoxelVertex{{offset.x, offset.y + voxel_size, offset.z},
                                   glm::vec4{1.0f},
                                   {uv.x, uv.y},
                                   ao.p3});

    vertices.push_back(
        VoxelVertex{{offset.x + voxel_size, offset.y + voxel_size, offset.z + voxel_size},
                    glm::vec4{1.0f},
                    {uv.z, uv.w},
                    ao.p4});
    vertices.push_back(VoxelVertex{{offset.x + voxel_size, offset.y + voxel_size, offset.z},
                                   glm::vec4{1.0f},
                                   {uv.z, uv.y},
                                   ao.p5});
    vertices.push_back(VoxelVertex{{offset.x, offset.y + voxel_size, offset.z},
                                   glm::vec4{1.0f},
                                   {uv.x, uv.y},
                                   ao.p6});
}

void ChunkMesh::createDownSide(std::vector<VoxelVertex> &vertices,
                               const glm::vec3 &offset,
                               const glm::vec4 &uv,
                               float voxel_size,
                               const AmbientOcclusion &ao)
{
    vertices.push_back(VoxelVertex{{offset.x + voxel_size, offset.y, offset.z + voxel_size},
                                   glm::vec4{1.0f},
                                   {uv.x, uv.w},
                                   ao.p1});
    vertices.push_back(VoxelVertex{{offset.x, offset.y, offset.z + voxel_size},
                                   glm::vec4{1.0f},
                                   {uv.z, uv.w},
                                   ao.p2});
    vertices.push_back(VoxelVertex{{offset.x + voxel_size, offset.y, offset.z},
                                   glm::vec4{1.0f},
                                   {uv.x, uv.y},
                                   ao.p3});

    vertices.push_back(VoxelVertex{{offset.x, offset.y, offset.z + voxel_size},
                                   glm::vec4{1.0f},
                                   {uv.z, uv.w},
                                   ao.p4});
    vertices.push_back(
        VoxelVertex{{offset.x, offset.y, offset.z}, glm::vec4{1.0f}, {uv.z, uv.y}, ao.p5});
    vertices.push_back(VoxelVertex{{offset.x + voxel_size, offset.y, offset.z},
                                   glm::vec4{1.0f},
                                   {uv.x, uv.y},
                                   ao.p6});
}

void ChunkMesh::createLeftSide(std::vector<VoxelVertex> &vertices,
                               const glm::vec3 &offset,
                               const glm::vec4 &uv,
                               float voxel_size,
                               const AmbientOcclusion &ao)
{
    vertices.push_back(
        VoxelVertex{{offset.x, offset.y, offset.z}, glm::vec4{1.0f}, {uv.x, uv.w}, ao.p1});
    vertices.push_back(VoxelVertex{{offset.x, offset.y, offset.z + voxel_size},
                                   glm::vec4{1.0f},
                                   {uv.z, uv.w},
                                   ao.p2});
    vertices.push_back(VoxelVertex{{offset.x, offset.y + voxel_size, offset.z},
                                   glm::vec4{1.0f},
                                   {uv.x, uv.y},
                                   ao.p3});

    vertices.push_back(VoxelVertex{{offset.x, offset.y, offset.z + voxel_size},
                                   glm::vec4{1.0f},
                                   {uv.z, uv.w},
                                   ao.p4});
    vertices.push_back(VoxelVertex{{offset.x, offset.y + voxel_size, offset.z + voxel_size},
                                   glm::vec4{1.0f},
                                   {uv.z, uv.y},
                                   ao.p5});
    vertices.push_back(VoxelVertex{{offset.x, offset.y + voxel_size, offset.z},
                                   glm::vec4{1.0f},
                                   {uv.x, uv.y},
                                   ao.p6});
}

void ChunkMesh::createRightSide(std::vector<VoxelVertex> &vertices,
                                const glm::vec3 &offset,
                                const glm::vec4 &uv,
                                float voxel_size,
                                const AmbientOcclusion &ao)
{
    vertices.push_back(VoxelVertex{{offset.x + voxel_size, offset.y, offset.z + voxel_size},
                                   glm::vec4{1.0f},
                                   {uv.x, uv.w},
                                   ao.p1});
    vertices.push_back(VoxelVertex{{offset.x + voxel_size, offset.y, offset.z},
                                   glm::vec4{1.0f},
                                   {uv.z, uv.w},
                                   ao.p2});
    vertices.push_back(
        VoxelVertex{{offset.x + voxel_size, offset.y + voxel_size, offset.z + voxel_size},
                    glm::vec4{1.0f},
                    {uv.x, uv.y},
                    ao.p3});

    vertices.push_back(VoxelVertex{{offset.x + voxel_size, offset.y, offset.z},
                                   glm::vec4{1.0f},
                                   {uv.z, uv.w},
                                   ao.p4});

    vertices.push_back(VoxelVertex{{offset.x + voxel_size, offset.y + voxel_size, offset.z},
                                   glm::vec4{1.0f},
                                   {uv.z, uv.y},
                                   ao.p5});
    vertices.push_back(
        VoxelVertex{{offset.x + voxel_size, offset.y + voxel_size, offset.z + voxel_size},
                    glm::vec4{1.0f},
                    {uv.x, uv.y},
                    ao.p6});
}

void ChunkMesh::createFrontSide(std::vector<VoxelVertex> &vertices,
                                const glm::vec3 &offset,
                                const glm::vec4 &uv,
                                float voxel_size,
                                const AmbientOcclusion &ao)
{
    vertices.push_back(VoxelVertex{{offset.x, offset.y, offset.z + voxel_size},
                                   glm::vec4{1.0f},
                                   {uv.x, uv.w},
                                   ao.p1});
    vertices.push_back(VoxelVertex{{offset.x + voxel_size, offset.y, offset.z + voxel_size},
                                   glm::vec4{1.0f},
                                   {uv.z, uv.w},
                                   ao.p2});
    vertices.push_back(VoxelVertex{{offset.x, offset.y + voxel_size, offset.z + voxel_size},
                                   glm::vec4{1.0f},
                                   {uv.x, uv.y},
                                   ao.p3});

    vertices.push_back(VoxelVertex{{offset.x + voxel_size, offset.y, offset.z + voxel_size},
                                   glm::vec4{1.0f},
                                   {uv.z, uv.w},
                                   ao.p4});
    vertices.push_back(
        VoxelVertex{{offset.x + voxel_size, offset.y + voxel_size, offset.z + voxel_size},
                    glm::vec4{1.0f},
                    {uv.z, uv.y},
                    ao.p5});
    vertices.push_back(VoxelVertex{{offset.x, offset.y + voxel_size, offset.z + voxel_size},
                                   glm::vec4{1.0f},
                                   {uv.x, uv.y},
                                   ao.p6});
}

void ChunkMesh::createBackSide(std::vector<VoxelVertex> &vertices,
                               const glm::vec3 &offset,
                               const glm::vec4 &uv,
                               float voxel_size,
                               const AmbientOcclusion &ao)
{
    vertices.push_back(VoxelVertex{{offset.x + voxel_size, offset.y, offset.z},
                                   glm::vec4{1.0f},
                                   {uv.x, uv.w},
                                   ao.p1});
    vertices.push_back(
        VoxelVertex{{offset.x, offset.y, offset.z}, glm::vec4{1.0f}, {uv.z, uv.w}, ao.p2});
    vertices.push_back(VoxelVertex{{offset.x + voxel_size, offset.y + voxel_size, offset.z},
                                   glm::vec4{1.0f},
                                   {uv.x, uv.y},
                                   ao.p3});

    vertices.push_back(
        VoxelVertex{{offset.x, offset.y, offset.z}, glm::vec4{1.0f}, {uv.z, uv.w}, ao.p4});
    vertices.push_back(VoxelVertex{{offset.x, offset.y + voxel_size, offset.z},
                                   glm::vec4{1.0f},
                                   {uv.z, uv.y},
                                   ao.p5});
    vertices.push_back(VoxelVertex{{offset.x + voxel_size, offset.y + voxel_size, offset.z},
                                   glm::vec4{1.0f},
                                   {uv.x, uv.y},
                                   ao.p6});
}

} // namespace eb
