#ifndef EB_GRAPHICS_TRANSFORMABLE_H
#define EB_GRAPHICS_TRANSFORMABLE_H

#include <glm/glm.hpp>

namespace eb {

class Transformable
{
public:
    Transformable();
    Transformable(const glm::mat4 &transform);
    virtual ~Transformable() = default;

    const glm::vec3 &getPosition() const;
    void setPosition(const glm::vec3 &position);
    void move(const glm::vec3 &vec);

    const glm::vec3 &getRotation() const;
    void setRotation(const glm::vec3 &rotation);
    void rotate(const glm::vec3 &vec);

    const glm::vec3 &getScale() const;
    void setScale(const glm::vec3 &scale);
    void scale(const glm::vec3 &vec);

    const glm::vec3 &getOrigin() const;
    void setOrigin(const glm::vec3 &origin);

    const glm::mat4 &getTransform() const;
    void setTransform(const glm::mat4 &transform);

private:
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
    glm::vec3 m_origin;

    mutable bool m_need_update_transform;
    mutable glm::mat4 m_transform;
};

} // namespace eb

#endif // EB_GRAPHICS_TRANSFORMABLE_H
