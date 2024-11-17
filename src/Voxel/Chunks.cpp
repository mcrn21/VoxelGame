#include "Chunks.h"

#include <spdlog/spdlog.h>

namespace eb {

Chunks::Chunks(const glm::i32vec3 &chunks_size,
               const glm::i32vec3 &chunk_size,
               float voxel_size,
               float texture_size,
               Engine *engine)
    : EngineObject{engine}
    , m_chunks_size{chunks_size}
    , m_chunk_size{chunk_size}
    , m_voxel_size{voxel_size}
    , m_texture_size{texture_size}
{
    m_chunks.resize(m_chunks_size.x * m_chunks_size.y * m_chunks_size.z);
    m_meshes.resize(m_chunks_size.x * m_chunks_size.y * m_chunks_size.z);

    for (int32_t y = 0; y < m_chunks_size.y; ++y) {
        for (int32_t z = 0; z < m_chunks_size.z; ++z) {
            for (int32_t x = 0; x < m_chunks_size.x; ++x) {
                auto chunk = std::make_unique<Chunk>(glm::i32vec3{x, y, z},
                                                     chunk_size,
                                                     voxel_size,
                                                     texture_size);
                m_chunks[(y * m_chunks_size.z + z) * m_chunks_size.x + x] = std::move(chunk);

                auto mesh = std::make_unique<ChunkMesh>(engine);
                m_meshes[(y * m_chunks_size.z + z) * m_chunks_size.x + x] = std::move(mesh);
            }
        }
    }
}

const glm::i32vec3 &Chunks::getChunksSize() const
{
    return m_chunks_size;
}

const glm::i32vec3 &Chunks::getChunkSize() const
{
    return m_chunk_size;
}

float Chunks::getVoxelSize() const
{
    return m_voxel_size;
}

float Chunks::getTextureSize() const
{
    return m_texture_size;
}

const Chunk *Chunks::getChunk(const glm::i32vec3 &index) const
{
    if (!containsChunk(index))
        return nullptr;
    return m_chunks[(index.y * m_chunks_size.z + index.z) * m_chunks_size.x + index.x].get();
}

const Chunk *Chunks::getChunkByGlobal(const glm::vec3 &coords) const
{
    return getChunk(static_cast<glm::i32vec3>(coords / m_voxel_size) / m_chunk_size);
}

const Voxel *Chunks::getVoxel(const glm::i32vec3 &index) const
{
    auto *chunk = getChunk(index / m_chunk_size);
    if (!chunk)
        return nullptr;
    return chunk->getVoxel(index % m_chunk_size);
}

const Voxel *Chunks::getVoxelByGlobal(const glm::vec3 &coords) const
{
    return getVoxel(static_cast<glm::i32vec3>(coords / m_voxel_size));
}

void Chunks::setVoxel(const glm::i32vec3 &index, int32_t id)
{
    auto chunk_coords = index / m_chunk_size;
    if (!containsChunk(chunk_coords))
        return;
    m_chunks[(chunk_coords.y * m_chunks_size.z + chunk_coords.z) * m_chunks_size.x + chunk_coords.x]
        ->setVoxel(index % m_chunk_size, id);
}

void Chunks::setVoxelByGlobal(const glm::vec3 &coords, int32_t id)
{
    setVoxel(static_cast<glm::i32vec3>(coords / m_voxel_size), id);
}

bool Chunks::isBlocked(const glm::i32vec3 &index) const
{
    auto *voxel = getVoxel(index);
    if (!voxel)
        return false;

    return containsVoxel(index) && voxel->id != 0;
}

bool Chunks::containsChunk(const glm::i32vec3 &index) const
{
    return index.x >= 0 && index.x < m_chunks_size.x && index.y >= 0 && index.y < m_chunks_size.y
           && index.z >= 0 && index.z < m_chunks_size.z;
}

bool Chunks::containsVoxel(const glm::i32vec3 &index) const
{
    return index.x >= 0 && index.x < (m_chunk_size.x * m_chunks_size.x) && index.y >= 0
           && index.y < (m_chunk_size.y * m_chunks_size.y) && index.z >= 0
           && index.z < (m_chunk_size.z * m_chunks_size.z);
}

glm::i32vec3 Chunks::getChunkCoordsByGlobal(const glm::vec3 &coords) const
{
    return static_cast<glm::i32vec3>(coords / m_voxel_size) / m_chunk_size;
}

int32_t Chunks::getChunkIndexByGlobal(const glm::vec3 &coords) const
{
    auto chunk_coords = static_cast<glm::i32vec3>(coords / m_voxel_size) / m_chunk_size;
    return (chunk_coords.y * m_chunks_size.z + chunk_coords.z) * m_chunks_size.x + chunk_coords.x;
}

const Voxel *Chunks::rayCast(glm::vec3 start,
                             glm::vec3 direction,
                             float max_dist,
                             glm::vec3 &end,
                             glm::vec3 &norm,
                             glm::vec3 &iend)
{
    float px = start.x;
    float py = start.y;
    float pz = start.z;

    float dx = direction.x;
    float dy = direction.y;
    float dz = direction.z;

    float t = 0.0f;
    int32_t ix = std::floor(px);
    int32_t iy = std::floor(py);
    int32_t iz = std::floor(pz);

    float step_x = (dx > 0.0f) ? 1.0f : -1.0f;
    float step_y = (dy > 0.0f) ? 1.0f : -1.0f;
    float step_z = (dz > 0.0f) ? 1.0f : -1.0f;

    float infinity = std::numeric_limits<float>::infinity();

    float tx_delta = (dx == 0.0f) ? infinity : std::abs(1.0f / dx);
    float ty_delta = (dy == 0.0f) ? infinity : std::abs(1.0f / dy);
    float tz_delta = (dz == 0.0f) ? infinity : std::abs(1.0f / dz);

    float x_dist = (step_x > 0) ? (ix + 1 - px) : (px - ix);
    float y_dist = (step_y > 0) ? (iy + 1 - py) : (py - iy);
    float z_dist = (step_z > 0) ? (iz + 1 - pz) : (pz - iz);

    float tx_max = (tx_delta < infinity) ? tx_delta * x_dist : infinity;
    float ty_max = (ty_delta < infinity) ? ty_delta * y_dist : infinity;
    float tz_max = (tz_delta < infinity) ? tz_delta * z_dist : infinity;

    int32_t stepped_index = -1;
    max_dist *= m_voxel_size;

    while (t <= max_dist) {
        auto *voxel = getVoxelByGlobal(glm::vec3{ix, iy, iz});

        if (voxel == nullptr || voxel->id != 0) {
            end.x = px + t * dx;
            end.y = py + t * dy;
            end.z = pz + t * dz;

            iend.x = ix;
            iend.y = iy;
            iend.z = iz;

            norm.x = norm.y = norm.z = 0.0f;
            if (stepped_index == 0)
                norm.x = -step_x;
            if (stepped_index == 1)
                norm.y = -step_y;
            if (stepped_index == 2)
                norm.z = -step_z;
            return voxel;
        }

        if (tx_max < ty_max) {
            if (tx_max < tz_max) {
                ix += step_x;
                t = tx_max;
                tx_max += tx_delta;
                stepped_index = 0;
            } else {
                iz += step_z;
                t = tz_max;
                tz_max += tz_delta;
                stepped_index = 2;
            }
        } else {
            if (ty_max < tz_max) {
                iy += step_y;
                t = ty_max;
                ty_max += ty_delta;
                stepped_index = 1;
            } else {
                iz += step_z;
                t = tz_max;
                tz_max += tz_delta;
                stepped_index = 2;
            }
        }
    }

    iend.x = ix;
    iend.y = iy;
    iend.z = iz;

    end.x = px + t * dx;
    end.y = py + t * dy;
    end.z = pz + t * dz;
    norm.x = norm.y = norm.z = 0.0f;

    return nullptr;
}

void Chunks::forEach(const std::function<void(const Chunk *, const glm::i32vec3 &)> &func) const {}

} // namespace eb
