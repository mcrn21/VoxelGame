#ifndef EB_GRAPHICS_3D_CAMERA_H
#define EB_GRAPHICS_3D_CAMERA_H

#include "../Common/Frustum.h"

#include <glm/glm.hpp>

using namespace glm;

namespace eb {

class Camera
{
public:
    Camera();
    Camera(vec3 position, float fov, float near, float far);
    ~Camera() = default;

    float getRatio() const;
    void setRatio(float ratio);

    float getNear() const;
    void setNear(float near);

    float getFar() const;
    void setFar(float far);

    const vec3 &getPosition() const;
    void setPosition(const vec3 &position);
    void move(const vec3 &position);

    const vec3 &getRotation() const;
    void setRotation(const vec3 &rotation);
    void rotate(const vec3 &rotation);

    const vec3 &getFront() const;
    const vec3 &getUp() const;
    const vec3 &getRight() const;

    float getFov() const;
    void setFov(float fov);

    bool isChanged() const;
    void resetChangeFlag();

    const mat4 &getRotationMat() const;
    const mat4 &getProjMat() const;
    const mat4 &getViewMat() const;
    const mat4 &getProjViewMat() const;
    const Frustum &getFrustum() const;

private:
    void update();

private:
    float m_ratio;
    float m_fov;
    float m_near;
    float m_far;
    vec3 m_position;
    vec3 m_rotation;
    vec3 m_front;
    vec3 m_up;
    vec3 m_right;

    bool m_need_update;
    mat4 m_rotation_mat;
    mat4 m_proj_mat;
    mat4 m_view_mat;
    mat4 m_proj_view_mat;
    Frustum m_frustum;

    bool m_changed;
};

} // namespace eb

#endif // EB_GRAPHICS_3D_CAMERA_H
