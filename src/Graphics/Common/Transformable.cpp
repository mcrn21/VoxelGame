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

const vec3 &Transformable::getPosition() const
{
    return m_position;
}

void Transformable::setPosition(const vec3 &position)
{
    m_position = position;
    m_need_update_transform = true;
}

void Transformable::move(const vec3 &vec)
{
    m_position += vec;
    m_need_update_transform = true;
}

const vec3 &Transformable::getRotation() const
{
    return m_rotation;
}

void Transformable::setRotation(const vec3 &rotation)
{
    m_rotation = rotation;
    m_need_update_transform = true;
}

void Transformable::rotate(const vec3 &vec)
{
    m_rotation += vec;
    m_need_update_transform = true;
}

const vec3 &Transformable::getScale() const
{
    return m_scale;
}

void Transformable::setScale(const vec3 &scale)
{
    m_scale = scale;
    m_need_update_transform = true;
}

void Transformable::scale(const vec3 &vec)
{
    m_scale += vec;
    m_need_update_transform = true;
}

const vec3 &Transformable::getOrigin() const
{
    return m_origin;
}

void Transformable::setOrigin(const vec3 &origin)
{
    m_origin = origin;
    m_need_update_transform = true;
}

const mat4 &Transformable::getTransform() const
{
    if (m_need_update_transform) {
        m_need_update_transform = false;
        const_cast<Transformable *>(this)->updateTransforms();
    }

    return m_transform;
}

void Transformable::updateTransforms()
{
    m_transform = mat4{1.0f};

    mat4 rotation_mat{1.0f};
    rotation_mat = glm::rotate(rotation_mat, m_rotation.z, vec3(0, 0, 1));
    rotation_mat = glm::rotate(rotation_mat, m_rotation.y, vec3(0, 1, 0));
    rotation_mat = glm::rotate(rotation_mat, m_rotation.x, vec3(1, 0, 0));

    m_transform = glm::translate(m_transform, m_position);
    m_transform = glm::translate(m_transform, -vec3(rotation_mat * vec4(m_origin, 1)));
    m_transform = glm::scale(m_transform, m_scale);

    m_transform *= rotation_mat;
}

} // namespace eb
