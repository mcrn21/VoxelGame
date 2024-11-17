#include "Transformable.h"

#include <glm/ext.hpp>

namespace eb {

Transformable::Transformable()
    : m_position{0.0f}
    , m_rotation{0.0f}
    , m_scale{1.0f}
    , m_origin{0.0f}
    , m_need_update_transform{true}
    , m_transform{1.0f}
{}

Transformable::Transformable(const glm::mat4 &transform)
    : m_position{0.0f}
    , m_rotation{0.0f}
    , m_scale{1.0f}
    , m_origin{0.0f}
    , m_need_update_transform{false}
    , m_transform{transform}
{}

const glm::vec3 &Transformable::getPosition() const
{
    return m_position;
}

void Transformable::setPosition(const glm::vec3 &position)
{
    m_position = position;
    m_need_update_transform = true;
}

void Transformable::move(const glm::vec3 &vec)
{
    m_position += vec;
    m_need_update_transform = true;
}

const glm::vec3 &Transformable::getRotation() const
{
    return m_rotation;
}

void Transformable::setRotation(const glm::vec3 &rotation)
{
    m_rotation = rotation;
    m_need_update_transform = true;
}

void Transformable::rotate(const glm::vec3 &vec)
{
    m_rotation += vec;
    m_need_update_transform = true;
}

const glm::vec3 &Transformable::getScale() const
{
    return m_scale;
}

void Transformable::setScale(const glm::vec3 &scale)
{
    m_scale = scale;
    m_need_update_transform = true;
}

void Transformable::scale(const glm::vec3 &vec)
{
    m_scale += vec;
    m_need_update_transform = true;
}

const glm::vec3 &Transformable::getOrigin() const
{
    return m_origin;
}

void Transformable::setOrigin(const glm::vec3 &origin)
{
    m_origin = origin;
    m_need_update_transform = true;
}

const glm::mat4 &Transformable::getTransform() const
{
    if (m_need_update_transform) {
        m_need_update_transform = false;

        m_transform = glm::mat4{1.0f};

        glm::mat4 rotation_mat{1.0f};
        rotation_mat = glm::rotate(rotation_mat, m_rotation.z, glm::vec3(0, 0, 1));
        rotation_mat = glm::rotate(rotation_mat, m_rotation.y, glm::vec3(0, 1, 0));
        rotation_mat = glm::rotate(rotation_mat, m_rotation.x, glm::vec3(1, 0, 0));

        m_transform = glm::translate(m_transform, m_position);
        m_transform = glm::translate(m_transform, -glm::vec3(rotation_mat * glm::vec4(m_origin, 1)));
        m_transform = glm::scale(m_transform, m_scale);

        m_transform *= rotation_mat;
    }

    return m_transform;
}

void Transformable::setTransform(const glm::mat4 &transform)
{
    m_transform = transform;
}

} // namespace eb
