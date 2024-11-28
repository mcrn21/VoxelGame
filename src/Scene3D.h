#ifndef EB_GRAPHICS_SCENE_H
#define EB_GRAPHICS_SCENE_H

#include "Graphics/3D/Camera.h"
#include "Graphics/3D/Lights.h"
#include "Graphics/Common/RenderTarget.h"
#include "System/Time.h"

#include <entt/entt.hpp>

namespace eb {

class Scene3D
{
public:
    Scene3D();
    ~Scene3D() = default;

    std::shared_ptr<Camera> getCamera() const;
    void setCamera(const std::shared_ptr<Camera> &camera);

    std::shared_ptr<Lights> getLights() const;
    void setLights(const std::shared_ptr<Lights> &lights);

    entt::registry &getRegistry();

    void updateProjectionMatrix(const i32vec2 &size);

    void update(const Time &elapsed);
    void draw(const Time &elapsed, const RenderTarget &render_target) const;

private:
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<Lights> m_lights;

    entt::registry m_registry;
};

} // namespace eb

#endif // EB_GRAPHICS_SCENE_H
