#include "Camera.h"
#include "../Engine.h"

#include <glm/ext.hpp>

namespace eb {

Camera::Camera()
    : m_ratio{1.0f}
    , m_position{0.0f}
    , m_rotation{0.0f}
    , m_up{0.0f, 1.0f, 0.0f}
    , m_fov{70.0f}
    , m_need_update{ROTATION | PROJECTION | VIEW}
    , m_rotation_mat{1.0f}
    , m_projection_mat{1.0f}
    , m_view_mat{1.0f}
{}

Camera::Camera(glm::vec3 position, float fov)
    : m_ratio{1.0f}
    , m_position{position}
    , m_rotation{0.0f}
    , m_up{0.0f, 1.0f, 0.0f}
    , m_fov{fov}
    , m_need_update{ROTATION | PROJECTION | VIEW}
    , m_rotation_mat{1.0f}
    , m_projection_mat{1.0f}
    , m_view_mat{1.0f}
{}

float Camera::getRatio() const
{
    return m_ratio;
}

void Camera::setRatio(float ratio)
{
    m_ratio = ratio;
    m_need_update |= PROJECTION;
}

const glm::vec3 &Camera::getPosition() const
{
    return m_position;
}

void Camera::setPosition(const glm::vec3 &position)
{
    m_position = position;
    m_need_update |= VIEW;
}

void Camera::move(const glm::vec3 &position)
{
    m_position += position;
    m_need_update |= VIEW;
}

const glm::vec3 &Camera::getRotation() const
{
    return m_rotation;
}

void Camera::setRotation(const glm::vec3 &rotation)
{
    m_rotation = rotation;
    m_need_update |= ROTATION | VIEW;
}

void Camera::rotate(const glm::vec3 &rotation)
{
    m_rotation += rotation;
    m_need_update |= ROTATION | VIEW;
}

const glm::vec3 &Camera::getFront() const
{
    if (m_need_update & VIEW) {
        m_need_update = m_need_update & ~VIEW;
        m_front = glm::vec3(getRotationMat() * glm::vec4(0, 0, -1, 1));
        m_view_mat = glm::lookAt(m_position, m_position + m_front, m_up);
    }

    return m_front;
}

float Camera::getFov() const
{
    return m_fov;
}

void Camera::setFov(float fov)
{
    m_fov = fov;
    m_need_update |= PROJECTION;
}

glm::mat4 Camera::getRotationMat() const
{
    if (m_need_update & ROTATION) {
        m_need_update = m_need_update & ~ROTATION;
        m_rotation_mat = glm::mat4{1.0f};
        m_rotation_mat = glm::rotate(m_rotation_mat, m_rotation.z, glm::vec3(0, 0, 1));
        m_rotation_mat = glm::rotate(m_rotation_mat, m_rotation.y, glm::vec3(0, 1, 0));
        m_rotation_mat = glm::rotate(m_rotation_mat, m_rotation.x, glm::vec3(1, 0, 0));
    }

    return m_rotation_mat;
}

glm::mat4 Camera::getProjectionMat() const
{
    if (m_need_update & PROJECTION) {
        m_need_update = m_need_update & ~PROJECTION;
        m_projection_mat = glm::perspective(m_fov, m_ratio, 0.1f, 1000.0f);
    }

    return m_projection_mat;
}

glm::mat4 Camera::getViewMat() const
{
    if (m_need_update & VIEW) {
        m_need_update = m_need_update & ~VIEW;
        m_front = glm::vec3(getRotationMat() * glm::vec4(0, 0, -1, 1));
        m_view_mat = glm::lookAt(m_position, m_position + m_front, m_up);
    }

    return m_view_mat;
}

} // namespace eb
