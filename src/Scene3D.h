#ifndef EB_GRAPHICS_SCENE_H
#define EB_GRAPHICS_SCENE_H

#include "EngineObject.h"
#include "Graphics/3D/Camera.h"
#include "Graphics/3D/Lights.h"
#include "Graphics/Common/CascadeShadowMap.h"
#include "Graphics/Common/RenderTarget.h"
#include "Graphics/Common/Shader.h"
#include "Graphics/Common/ShadowMap.h"
#include "System/Time.h"

#include <entt/entt.hpp>

namespace eb {

class Scene3D : public EngineObject
{
public:
    enum ShadowMapType { NO_SHADOWS, SIMPLE_SHADOW_MAP, CASCADE_SHADOW_MAP };

    Scene3D(Engine *engine);
    ~Scene3D() = default;

    std::shared_ptr<Camera> getCamera() const;
    void setCamera(const std::shared_ptr<Camera> &camera);

    std::shared_ptr<Lights> getLights() const;
    void setLights(const std::shared_ptr<Lights> &lights);

    entt::registry &getRegistry();

    void setShadowMap(ShadowMapType shadow_map_type,
                      const i32vec2 &size = vec2{0.0f},
                      const std::vector<float> &planes = {});

    std::shared_ptr<Shader> getMainShader() const;
    void setMainShader(const std::shared_ptr<Shader> &shader);

    std::shared_ptr<Shader> getCSMShader() const;
    void setCSMShader(const std::shared_ptr<Shader> &shader);

    void updateProjectionMatrix(const i32vec2 &size);

    void update(const Time &elapsed);
    void updateTick(const Time &elapsed);
    void draw(const Time &elapsed, const RenderTarget &render_target) const;

private:
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<Lights> m_lights;

    entt::registry m_registry;

    std::shared_ptr<Shader> m_main_shader;
    std::shared_ptr<Shader> m_csm_shader;

    // Shadows
    ShadowMapType m_shadow_map_type;
    mutable CascadeShadowMap2 m_cascade_shadow_map;
    mutable std::unique_ptr<ShadowMapBase> m_shadow_map;
};

} // namespace eb

#endif // EB_GRAPHICS_SCENE_H
