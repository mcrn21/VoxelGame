#include "Chunks.h"

#include <glm/gtc/noise.hpp>
#include <spdlog/spdlog.h>

namespace eb {

Chunks::Chunks(const glm::i32vec3 &chunks_size,
               const glm::i32vec3 &chunk_size,
               float voxel_size,
               float texture_size,
               const std::shared_ptr<Texture> &atlas_texture,
               Engine *engine)
    : EngineObject{engine}
    , m_chunks_size{chunks_size}
    , m_chunk_size{chunk_size}
    , m_voxel_size{voxel_size}
    , m_texture_size{texture_size}
    , m_atlas_texture{atlas_texture}
    , m_chunks_modfied{false}
{
    m_chunk_states.resize(m_chunks_size.x * m_chunks_size.y * m_chunks_size.z);

    // Create chunks
    for (int32_t y = 0; y < m_chunks_size.y; ++y) {
        for (int32_t z = 0; z < m_chunks_size.z; ++z) {
            for (int32_t x = 0; x < m_chunks_size.x; ++x) {
                glm::i32vec3 chunk_coords = {x, y, z};

                auto chunk_state = std::make_unique<ChunkState>(chunk_coords,
                                                                this,
                                                                atlas_texture,
                                                                engine);
                chunk_state->mesh->setPosition(static_cast<glm::vec3>(chunk_coords)
                                               * static_cast<glm::vec3>(chunk_size) * voxel_size);

                m_chunk_states[chunkCoordsToIndex({x, y, z})] = std::move(chunk_state);
            }
        }
    }

    for (int32_t y = 0; y < m_chunks_size.y; ++y) {
        for (int32_t z = 0; z < m_chunks_size.z; ++z) {
            for (int32_t x = 0; x < m_chunks_size.x; ++x) {
                glm::i32vec3 chunk_coords = {x, y, z};
                setChunkData(chunk_coords);
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

std::shared_ptr<Texture> Chunks::getAtlasTexture() const
{
    return m_atlas_texture;
}

Chunk *Chunks::getChunk(const glm::i32vec3 &chunk_coords)
{
    if (!containsChunk(chunk_coords))
        return nullptr;
    return m_chunk_states[chunkCoordsToIndex(chunk_coords)]->chunk.get();
}

Chunk *Chunks::getChunkByVoxel(const glm::i32vec3 &voxel_coords)
{
    if (!containsVoxel(voxel_coords))
        return nullptr;

    auto chunk_coords = voxel_coords / m_chunk_size;
    if (!containsChunk(chunk_coords))
        return nullptr;

    return m_chunk_states[chunkCoordsToIndex(chunk_coords)]->chunk.get();
}

Chunk *Chunks::getChunkByGlobal(const glm::vec3 &global_coords)
{
    return getChunkByVoxel(static_cast<glm::i32vec3>(global_coords / m_voxel_size));
}

const Voxel *Chunks::getVoxel(const glm::i32vec3 &voxel_coords) const
{
    if (!containsVoxel(voxel_coords))
        return nullptr;
    auto chunk_coords = voxel_coords / m_chunk_size;
    return m_chunk_states[chunkCoordsToIndex(chunk_coords)]->chunk->getVoxel(voxel_coords
                                                                             % m_chunk_size);
}

const Voxel *Chunks::getVoxelByGlobal(const glm::vec3 &global_coords) const
{
    return getVoxel(static_cast<glm::i32vec3>(global_coords / m_voxel_size));
}

void Chunks::setVoxel(const glm::i32vec3 &voxel_coords, const Voxel &voxel)
{
    if (!containsVoxel(voxel_coords))
        return;

    auto chunk_coords = voxel_coords / m_chunk_size;
    auto local_voxel_coords = voxel_coords % m_chunk_size;

    if (local_voxel_coords.x == 0 && containsChunk(chunk_coords + glm::i32vec3{-1, 0, 0})) {
        m_chunk_states[chunkCoordsToIndex(chunk_coords + glm::i32vec3{-1, 0, 0})]->chunk->m_modified
            = true;
        m_chunks_modfied = true;
    }

    if (local_voxel_coords.x == (m_chunk_size.x - 1)
        && containsChunk(chunk_coords + glm::i32vec3{1, 0, 0})) {
        m_chunk_states[chunkCoordsToIndex(chunk_coords + glm::i32vec3{1, 0, 0})]->chunk->m_modified
            = true;
        m_chunks_modfied = true;
    }

    if (local_voxel_coords.y == 0 && containsChunk(chunk_coords + glm::i32vec3{0, -1, 0})) {
        m_chunk_states[chunkCoordsToIndex(chunk_coords + glm::i32vec3{0, -1, 0})]->chunk->m_modified
            = true;
        m_chunks_modfied = true;
    }

    if (local_voxel_coords.y == (m_chunk_size.y - 1)
        && containsChunk(chunk_coords + glm::i32vec3{0, 1, 0})) {
        m_chunk_states[chunkCoordsToIndex(chunk_coords + glm::i32vec3{0, 1, 0})]->chunk->m_modified
            = true;
        m_chunks_modfied = true;
    }

    if (local_voxel_coords.z == 0 && containsChunk(chunk_coords + glm::i32vec3{0, 0, -1})) {
        m_chunk_states[chunkCoordsToIndex(chunk_coords + glm::i32vec3{0, 0, -1})]->chunk->m_modified
            = true;
        m_chunks_modfied = true;
    }

    if (local_voxel_coords.z == (m_chunk_size.z - 1)
        && containsChunk(chunk_coords + glm::i32vec3{0, 0, 1})) {
        m_chunk_states[chunkCoordsToIndex(chunk_coords + glm::i32vec3{0, 0, 1})]->chunk->m_modified
            = true;
        m_chunks_modfied = true;
    }

    m_chunk_states[chunkCoordsToIndex(chunk_coords)]->chunk->setVoxel(voxel_coords % m_chunk_size,
                                                                      voxel);
}

void Chunks::setVoxelByGlobal(const glm::vec3 &global_coords, const Voxel &voxel)
{
    setVoxel(static_cast<glm::i32vec3>(global_coords / m_voxel_size), voxel);
}

uint8_t Chunks::getLight(const glm::i32vec3 &voxel_coords, int32_t channel) const
{
    if (!containsVoxel(voxel_coords))
        return 0;

    return m_chunk_states[chunkCoordsToIndex(voxel_coords / m_chunk_size)]
        ->chunk->getLightmap()
        .get(voxel_coords % m_chunk_size, channel);
}

bool Chunks::isVoxelBlocked(const glm::i32vec3 &voxel_coords) const
{
    auto *voxel = getVoxel(voxel_coords);
    return !voxel ? false : voxel->id != 0;
}

bool Chunks::containsChunk(const glm::i32vec3 &chunk_coords) const
{
    return chunk_coords.x >= 0 && chunk_coords.x < m_chunks_size.x && chunk_coords.y >= 0
           && chunk_coords.y < m_chunks_size.y && chunk_coords.z >= 0
           && chunk_coords.z < m_chunks_size.z;
}

bool Chunks::containsVoxel(const glm::i32vec3 &voxel_coords) const
{
    return voxel_coords.x >= 0 && voxel_coords.x < (m_chunk_size.x * m_chunks_size.x)
           && voxel_coords.y >= 0 && voxel_coords.y < (m_chunk_size.y * m_chunks_size.y)
           && voxel_coords.z >= 0 && voxel_coords.z < (m_chunk_size.z * m_chunks_size.z);
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

void Chunks::forEachVoxels(
    const std::function<void(const glm::i32vec3 &, const glm::i32vec3 &)> &func) const
{
    if (!func)
        return;

    glm::i32vec3 voxel_coords{0};
    for (voxel_coords.y = 0; voxel_coords.y < m_chunk_size.y * m_chunks_size.y; ++voxel_coords.y) {
        for (voxel_coords.z = 0; voxel_coords.z < m_chunk_size.z * m_chunks_size.z;
             ++voxel_coords.z) {
            for (voxel_coords.x = 0; voxel_coords.x < m_chunk_size.x * m_chunks_size.x;
                 ++voxel_coords.x) {
                func(voxel_coords % m_chunk_size, voxel_coords);
            }
        }
    }
}

void Chunks::forEachVoxelsInChunk(
    const glm::i32vec3 &chunk_coords,
    const std::function<void(const glm::i32vec3 &, const glm::i32vec3 &)> &func) const
{
    if (!containsChunk(chunk_coords) || !func)
        return;

    glm::i32vec3 voxel_offsets = chunk_coords * m_chunk_size;
    glm::i32vec3 voxel_coords{0};
    for (voxel_coords.y = 0; voxel_coords.y < m_chunk_size.y; ++voxel_coords.y) {
        for (voxel_coords.z = 0; voxel_coords.z < m_chunk_size.z; ++voxel_coords.z) {
            for (voxel_coords.x = 0; voxel_coords.x < m_chunk_size.x; ++voxel_coords.x) {
                func(voxel_coords, voxel_offsets + voxel_coords);
            }
        }
    }
}

void Chunks::update()
{
    if (m_chunks_modfied) {
        m_chunks_modfied = false;
        for (int32_t i = 0; i < m_chunk_states.size(); ++i) {
            if (m_chunk_states[i]->chunk->m_modified == true) {
                m_chunk_states[i]->chunk->m_modified = false;
                m_chunk_states[i]->mesh->create(this, m_chunk_states[i]->chunk->getPosition());
            }
        }
    }
}

void Chunks::draw(const RenderTarget &render_target) const
{
    for (const auto &chunk_state : m_chunk_states)
        render_target.draw(*chunk_state->mesh);
}

int32_t Chunks::chunkCoordsToIndex(const glm::i32vec3 &chunk_coords) const
{
    return (chunk_coords.y * m_chunks_size.z + chunk_coords.z) * m_chunks_size.x + chunk_coords.x;
}

void Chunks::setChunkData(const glm::i32vec3 &chunk_coords)
{
    forEachVoxelsInChunk(chunk_coords,
                         [this](const glm::i32vec3 &voxel_coords_in_chunk,
                                const glm::i32vec3 &voxel_coords) {
                             int32_t real_x = voxel_coords.x * m_chunk_size.x;
                             int32_t real_z = voxel_coords.z * m_chunk_size.z;
                             int32_t real_y = voxel_coords.y * m_chunk_size.y;

                             // float height = (glm::perlin(
                             //                     glm::vec2(real_x * 0.002625f, real_z * 0.00225f))
                             //                 + 1.0f)
                             //                * 0.6f;

                             float height = (glm::perlin(glm::vec3{real_x * 0.005625f,
                                                                   real_y * 0.009625f,
                                                                   real_z * 0.00525f})
                                             + 0.6f)
                                            * 0.8f;

                             int32_t id = (static_cast<float>(voxel_coords.y) / m_chunk_size.y)
                                          < height;

                             if (real_y <= 2)
                                 id = 2;

                             setVoxel(voxel_coords, Voxel{id});
                         });
}

} // namespace eb
