#include "LightSolver.h"
#include "../Voxel/Chunks.h"

namespace eb {

LightSolver::LightSolver(Chunks *chunks, int32_t channel)
    : m_chunks{chunks}
    , m_channel{channel}
{}

void LightSolver::add(const glm::i32vec3 &coords)
{
    add(coords, m_chunks->getLight(coords, m_channel));
}

void LightSolver::add(const glm::i32vec3 &coords, int32_t emission)
{
    if (emission <= 1)
        return;
    LightEntry entry;
    entry.position = coords;
    entry.light = emission;
    m_add_queue.push(entry);

    Chunk *chunk = m_chunks->getChunkByVoxel(coords);
    chunk->m_modified = true;
    chunk->getLightmap().set(coords % m_chunks->getChunkSize(), m_channel, entry.light);
}

void LightSolver::remove(const glm::i32vec3 &coords)
{
    Chunk *chunk = m_chunks->getChunkByVoxel(coords);
    if (chunk == nullptr)
        return;

    int32_t light = chunk->getLightmap().get(coords % m_chunks->getChunkSize(), m_channel);
    if (light == 0) {
        return;
    }

    LightEntry entry;
    entry.position = coords;
    entry.light = light;
    m_remove_queue.push(entry);

    chunk->getLightmap().set(coords % m_chunks->getChunkSize(), m_channel, 0);
}

void LightSolver::solve()
{
    static const int32_t coords[] = {0, 0, 1, 0, 0, -1, 0, 1, 0, 0, -1, 0, 1, 0, 0, -1, 0, 0};

    while (!m_remove_queue.empty()) {
        LightEntry entry = m_remove_queue.front();
        m_remove_queue.pop();

        for (size_t i = 0; i < 6; i++) {
            glm::i32vec3 voxel_coords = entry.position
                                        + glm::i32vec3{coords[i * 3 + 0],
                                                       coords[i * 3 + 1],
                                                       coords[i * 3 + 2]};

            Chunk *chunk = m_chunks->getChunkByVoxel(voxel_coords);
            if (chunk) {
                int32_t light = m_chunks->getLight(voxel_coords, m_channel);
                if (light != 0 && light == entry.light - 1) {
                    LightEntry nentry;
                    nentry.position = voxel_coords;
                    nentry.light = light;
                    m_remove_queue.push(nentry);
                    chunk->getLightmap().set(voxel_coords % m_chunks->getChunkSize(), m_channel, 0);
                    chunk->m_modified = true;
                } else if (light >= entry.light) {
                    LightEntry nentry;
                    nentry.position = voxel_coords;
                    nentry.light = light;
                    m_add_queue.push(nentry);
                }
            }
        }
    }

    while (!m_add_queue.empty()) {
        LightEntry entry = m_add_queue.front();
        m_add_queue.pop();

        if (entry.light <= 1)
            continue;

        for (size_t i = 0; i < 6; i++) {
            glm::i32vec3 voxel_coords = entry.position
                                        + glm::i32vec3{coords[i * 3 + 0],
                                                       coords[i * 3 + 1],
                                                       coords[i * 3 + 2]};
            Chunk *chunk = m_chunks->getChunkByVoxel(voxel_coords);
            if (chunk) {
                int32_t light = m_chunks->getLight(voxel_coords, m_channel);
                auto *voxel = m_chunks->getVoxel(voxel_coords);
                if (voxel->id == 0 && light + 2 <= entry.light) {
                    chunk->getLightmap().set(voxel_coords % m_chunks->getChunkSize(),
                                             m_channel,
                                             entry.light - 1);
                    chunk->m_modified = true;
                    LightEntry nentry;
                    nentry.position = voxel_coords;
                    nentry.light = entry.light - 1;
                    m_add_queue.push(nentry);
                }
            }
        }
    }
}

} // namespace eb
