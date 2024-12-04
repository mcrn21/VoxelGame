#ifndef EB_GRAPHICS_FRUSTUM_H
#define EB_GRAPHICS_FRUSTUM_H

#include <glm/glm.hpp>

#include <array>

using namespace glm;

namespace eb {

class Frustum
{
public:
    Frustum();
    ~Frustum() = default;

    const std::array<vec3, 8> &getPoints() const;

    void update(const glm::mat4 &proj_view_mat);

private:
    std::array<vec3, 8> m_points;
};

} // namespace eb

#endif // EB_GRAPHICS_FRUSTUM_H
