#ifndef EB_GRAPHICS_3D_CAMERA_H
#define EB_GRAPHICS_3D_CAMERA_H

#include <glm/glm.hpp>

using namespace glm;

namespace eb {

class Camera
{
public:
    enum Update { NO_UPDATE = 0x0, ROTATION = 0x1, PROJECTION = 0x2, VIEW = 0x4 };

    Camera();
    Camera(vec3 position, float fov);
    ~Camera() = default;

    float getRatio() const;
    void setRatio(float ratio);

    const vec3 &getPosition() const;
    void setPosition(const vec3 &position);
    void move(const vec3 &position);

    const vec3 &getRotation() const;
    void setRotation(const vec3 &rotation);
    void rotate(const vec3 &rotation);

    const vec3 &getFront() const;

    float getFov() const;
    void setFov(float fov);

    mat4 getRotationMat() const;
    mat4 getProjectionMat() const;
    mat4 getViewMat() const;

private:
    float m_ratio;
    vec3 m_position;
    vec3 m_rotation;
    mutable vec3 m_front;
    mutable vec3 m_up;
    float m_fov;

    mutable int32_t m_need_update;
    mutable mat4 m_rotation_mat;
    mutable mat4 m_projection_mat;
    mutable mat4 m_view_mat;
};

} // namespace eb

#endif // EB_GRAPHICS_3D_CAMERA_H
