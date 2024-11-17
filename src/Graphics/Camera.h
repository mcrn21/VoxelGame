#ifndef EB_GRAPHICS_CAMERA_H
#define EB_GRAPHICS_CAMERA_H

#include <glm/glm.hpp>

namespace eb {

class Camera
{
public:
    enum Update { NO_UPDATE = 0x0, ROTATION = 0x1, PROJECTION = 0x2, VIEW = 0x4 };

    Camera();
    Camera(glm::vec3 position, float fov);
    ~Camera() = default;

    float getRatio() const;
    void setRatio(float ratio);

    const glm::vec3 &getPosition() const;
    void setPosition(const glm::vec3 &position);
    void move(const glm::vec3 &position);

    const glm::vec3 &getRotation() const;
    void setRotation(const glm::vec3 &rotation);
    void rotate(const glm::vec3 &rotation);

    const glm::vec3 &getFront() const;

    float getFov() const;
    void setFov(float fov);

    glm::mat4 getRotationMat() const;
    glm::mat4 getProjectionMat() const;
    glm::mat4 getViewMat() const;

private:
    float m_ratio;
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    mutable glm::vec3 m_front;
    mutable glm::vec3 m_up;
    float m_fov;

    mutable int32_t m_need_update;
    mutable glm::mat4 m_rotation_mat;
    mutable glm::mat4 m_projection_mat;
    mutable glm::mat4 m_view_mat;
};

} // namespace eb

#endif // EB_GRAPHICS_CAMERA_H
