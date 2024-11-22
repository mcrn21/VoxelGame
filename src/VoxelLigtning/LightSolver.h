#ifndef EB_VOXELLIGHTNING_LIGHTSOLVER_H
#define EB_VOXELLIGHTNING_LIGHTSOLVER_H

#include <glm/glm.hpp>

#include <queue>
#include <stdint.h>

namespace eb {

class Chunks;

struct LightEntry
{
    glm::i32vec3 position;
    uint16_t light;
};

class LightSolver
{
public:
    LightSolver(Chunks *chunks, int32_t channel);
    ~LightSolver() = default;

    void add(const glm::i32vec3 &coords);
    void add(const glm::i32vec3 &coords, int32_t emission);
    void remove(const glm::i32vec3 &coords);
    void solve();

private:
    Chunks *m_chunks;
    int32_t m_channel;
    std::queue<LightEntry> m_add_queue;
    std::queue<LightEntry> m_remove_queue;
};

} // namespace eb

#endif // EB_VOXELLIGHTNING_LIGHTSOLVER_H
