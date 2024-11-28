#ifndef EB_GRAPHICS_TRANSFORMABLE_H
#define EB_GRAPHICS_TRANSFORMABLE_H

#include <glm/glm.hpp>

using namespace glm;

namespace eb {

class Transformable
{
public:
    Transformable();
    virtual ~Transformable() = default;

    const vec3 &getPosition() const;
    void setPosition(const vec3 &position);
    void move(const vec3 &vec);

    const vec3 &getRotation() const;
    void setRotation(const vec3 &rotation);
    void rotate(const vec3 &vec);

    const vec3 &getScale() const;
    void setScale(const vec3 &scale);
    void scale(const vec3 &vec);

    const vec3 &getOrigin() const;
    void setOrigin(const vec3 &origin);

    const mat4 &getTransform() const;

private:
    void updateTransforms();

private:
    vec3 m_position;
    vec3 m_rotation;
    vec3 m_scale;
    vec3 m_origin;

    mutable bool m_need_update_transform;
    mat4 m_transform;
};

} // namespace eb

#endif // EB_GRAPHICS_TRANSFORMABLE_H
