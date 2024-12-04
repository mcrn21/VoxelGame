#ifndef EB_GRAPHICS_CASCADESHADOWMAP_H
#define EB_GRAPHICS_CASCADESHADOWMAP_H

#include "../3D/Camera.h"
#include "../3D/Lights.h"
#include "DepthRenderTextureArray.h"
#include "Frustum.h"
#include "GLBuffer.h"

#include <glm/glm.hpp>

#include <memory>

using namespace glm;

namespace eb {

class CascadeShadowMap2
{
public:
    CascadeShadowMap2();
    ~CascadeShadowMap2();

    void create(const vec2 &size,
                const std::vector<float> &cascade_levels,
                const std::shared_ptr<Camera> &camera,
                const std::shared_ptr<Lights> &lights);
    void destroy();

    const DepthRenderTextureArray &getRenderTexture() const;
    const GLBuffer &getShadowDataBuffer() const;

    void update();

private:
    struct CascadeCache
    {
        Frustum frustum;
        mat4 proj{1.0f};
        mat4 view{1.0f};
        mat4 proj_view{1.0f};
        vec3 center{0.0f};
        vec3 min{0.0f};
        vec3 max{0.0f};
        float near{0.0f};
        float far{0.0f};
    };

    DepthRenderTextureArray m_render_texture;
    GLBuffer m_shadow_data_buffer;

    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<Lights> m_lights;

    std::vector<float> m_cascade_levels;
    std::vector<CascadeCache> m_cascade_cache;
};

} // namespace eb

#endif // EB_GRAPHICS_CASCADESHADOWMAP_H
