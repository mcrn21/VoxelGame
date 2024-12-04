#include "Frustum.h"

namespace eb {

Frustum::Frustum()
    : m_points{vec3{0.0f}}
{}

const std::array<vec3, 8> &Frustum::getPoints() const
{
    return m_points;
}

void Frustum::update(const mat4 &proj_view_mat)
{
    auto inv = glm::inverse(proj_view_mat);

    for (int32_t x = 0; x < 2; ++x) {
        for (int32_t y = 0; y < 2; ++y) {
            for (int32_t z = 0; z < 2; ++z) {
                vec4 pt = inv * vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                m_points[(x * 2 + y) * 2 + z] = pt / pt.w;
            }
        }
    }
}

} // namespace eb
