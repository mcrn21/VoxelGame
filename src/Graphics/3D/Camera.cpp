#include "Camera.h"

#include <glm/ext.hpp>

namespace eb {

Camera::Camera()
    : m_ratio{1.0f}
    , m_fov{70.0f}
    , m_near{0.1f}
    , m_far{100.0f}
    , m_position{0.0f}
    , m_rotation{0.0f}
    , m_up{0.0f, 1.0f, 0.0f}
    , m_right{1.0f, 0.0f, 0.0f}
    , m_need_update{true}
    , m_proj_mat{1.0f}
    , m_view_mat{1.0f}
    , m_proj_view_mat{1.0f}
    , m_changed{true}
{}

Camera::Camera(vec3 position, float fov, float near, float far)
    : m_ratio{1.0f}
    , m_fov{fov}
    , m_near{near}
    , m_far{far}
    , m_position{position}
    , m_rotation{0.0f}
    , m_up{0.0f, 1.0f, 0.0f}
    , m_need_update{true}
    , m_proj_view_mat{1.0f}
    , m_changed{true}
{}

float Camera::getRatio() const
{
    return m_ratio;
}

void Camera::setRatio(float ratio)
{
    m_ratio = ratio;
    m_need_update = true;
    m_changed = true;
}

float Camera::getNear() const
{
    return m_near;
}

void Camera::setNear(float near)
{
    m_near = near;
    m_need_update = true;
    m_changed = true;
}

float Camera::getFar() const
{
    return m_far;
}

void Camera::setFar(float far)
{
    m_far = far;
    m_need_update = true;
    m_changed = true;
}

const vec3 &Camera::getPosition() const
{
    return m_position;
}

void Camera::setPosition(const vec3 &position)
{
    m_position = position;
    m_need_update = true;
    m_changed = true;
}

void Camera::move(const vec3 &position)
{
    m_position += position;
    m_need_update = true;
    m_changed = true;
}

const vec3 &Camera::getRotation() const
{
    return m_rotation;
}

void Camera::setRotation(const vec3 &rotation)
{
    m_rotation = rotation;
    m_need_update = true;
    m_changed = true;
}

void Camera::rotate(const vec3 &rotation)
{
    m_rotation += rotation;
    m_need_update = true;
    m_changed = true;
}

const vec3 &Camera::getFront() const
{
    if (m_need_update)
        const_cast<Camera *>(this)->update();

    return m_front;
}

const vec3 &Camera::getUp() const
{
    if (m_need_update)
        const_cast<Camera *>(this)->update();

    return m_up;
}

const vec3 &Camera::getRight() const
{
    if (m_need_update)
        const_cast<Camera *>(this)->update();

    return m_right;
}

float Camera::getFov() const
{
    return m_fov;
}

void Camera::setFov(float fov)
{
    m_fov = fov;
    m_need_update = true;
}

bool Camera::isChanged() const
{
    return m_changed;
}

void Camera::resetChangeFlag()
{
    m_changed = false;
}

const mat4 &Camera::getRotationMat() const
{
    if (m_need_update)
        const_cast<Camera *>(this)->update();

    return m_rotation_mat;
}

const mat4 &Camera::getProjMat() const
{
    if (m_need_update)
        const_cast<Camera *>(this)->update();

    return m_proj_mat;
}

const mat4 &Camera::getViewMat() const
{
    if (m_need_update)
        const_cast<Camera *>(this)->update();

    return m_view_mat;
}

const mat4 &Camera::getProjViewMat() const
{
    if (m_need_update)
        const_cast<Camera *>(this)->update();

    return m_proj_view_mat;
}

const Frustum &Camera::getFrustum() const
{
    if (m_need_update)
        const_cast<Camera *>(this)->update();

    return m_frustum;
}

void Camera::update()
{
    m_need_update = false;

    m_rotation_mat = mat4{1.0f};
    m_rotation_mat = glm::rotate(m_rotation_mat, m_rotation.z, vec3(0.0f, 0.0f, 1.0f));
    m_rotation_mat = glm::rotate(m_rotation_mat, m_rotation.y, vec3(0.0f, 1.0f, 0.0f));
    m_rotation_mat = glm::rotate(m_rotation_mat, m_rotation.x, vec3(1.0f, 0.0f, 0.0f));

    m_front = vec3(m_rotation_mat * vec4(0.0f, 0.0f, -1.0f, 1.0f));
    m_up = vec3(m_rotation_mat * vec4(0.0f, 1.0f, 0.0f, 1.0f));
    m_right = vec3(m_rotation_mat * vec4(1.0f, 0.0f, 0.0f, 1.0f));

    m_proj_mat = glm::perspective(glm::radians(m_fov), m_ratio, m_near, m_far);
    m_view_mat = glm::lookAt(m_position, m_position + m_front, m_up);

    m_proj_view_mat = m_proj_mat * m_view_mat;

    m_frustum.update(m_proj_view_mat);
}

} // namespace eb
