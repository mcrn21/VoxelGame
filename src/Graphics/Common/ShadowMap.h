#ifndef EB_GRAPHICS_SHADOWMAP_H
#define EB_GRAPHICS_SHADOWMAP_H

#include "../3D/Camera.h"
#include "../3D/Lights.h"
#include "DepthRenderTexture.h"
#include "DepthRenderTextureArray.h"
#include "GLBuffer.h"
#include "RenderTarget.h"
#include "Shader.h"

#include <glm/glm.hpp>

#include <memory>

using namespace glm;

namespace eb {

class ShadowMapBase : public RenderTarget
{
public:
    struct LightSpace
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

    ShadowMapBase();
    ~ShadowMapBase() = default;

    std::shared_ptr<Shader> getShader() const;

    virtual void update(const std::shared_ptr<Camera> &, const std::shared_ptr<Lights> &) = 0;
    virtual void applyToSceneShader(const std::shared_ptr<Shader> &scene_shader) = 0;

    virtual void saveToFile(const std::string &file_name){};

protected:
    void calculateLightSpace(LightSpace &light_space,
                             const std::shared_ptr<Camera> &camera,
                             const std::shared_ptr<Lights> &lights);

    virtual void createSpaceLights(){};

protected:
    std::shared_ptr<Shader> m_shader;
};

class ShadowMap : public ShadowMapBase
{
public:
    ShadowMap();
    ~ShadowMap() = default;

    void create(const i32vec2 &size, float near, float far);
    bool isValid() const;
    void destroy();

    void update(const std::shared_ptr<Camera> &camera, const std::shared_ptr<Lights> &lights);
    void applyToSceneShader(const std::shared_ptr<Shader> &scene_shader);

    void saveToFile(const std::string &file_name);

    void clear(const vec4 &color = vec4{0.0f}) const;
    void display() const;

private:
    DepthRenderTexture m_render_texture;
    LightSpace m_light_space;
    GLBuffer m_light_space_buffer;
};

class CascadeShadowMap : public ShadowMapBase
{
public:
    CascadeShadowMap();
    ~CascadeShadowMap() = default;

    void create(const i32vec2 &size, const std::vector<float> planes);
    bool isValid() const;
    void destroy();

    void update(const std::shared_ptr<Camera> &camera, const std::shared_ptr<Lights> &lights);
    void applyToSceneShader(const std::shared_ptr<Shader> &scene_shader);

    void saveToFile(const std::string &file_name);

    void clear(const vec4 &color = vec4{0.0f}) const;
    void display() const;

private:
    DepthRenderTextureArray m_render_texture;
    GLBuffer m_light_spaces_buffer;
    std::vector<LightSpace> m_light_spaces;
};

} // namespace eb

#endif // EB_GRAPHICS_SHADOWMAP_H
