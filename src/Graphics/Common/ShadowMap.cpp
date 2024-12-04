#include "ShadowMap.h"
#include "DefaultShaders.h"

#include <glm/ext.hpp>

#define CASCADE_SHADOW_MAP_MAX_MATRICES 16
#define FLOAT_ARRAY_ITEM_SIZE 16

namespace eb {

ShadowMapBase::ShadowMapBase() {}

std::shared_ptr<Shader> ShadowMapBase::getShader() const
{
    return m_shader;
}

void ShadowMapBase::calculateLightSpace(LightSpace &light_space,
                                        const std::shared_ptr<Camera> &camera,
                                        const std::shared_ptr<Lights> &lights)
{
    const auto proj = glm::perspective(glm::radians(camera->getFov()),
                                       camera->getRatio(),
                                       light_space.near,
                                       light_space.far);

    light_space.frustum.update(proj * camera->getViewMat());
    const auto &corners = light_space.frustum.getPoints();

    light_space.center = vec3{0.0f, 0.0f, 0.0f};
    for (const auto &v : corners) {
        light_space.center += glm::vec3(v);
    }
    light_space.center /= corners.size();

    light_space.view = glm::lookAt(light_space.center
                                       - glm::normalize(lights->getWorldLightDirection()),
                                   light_space.center,
                                   glm::vec3(0.0f, 1.0f, 0.0f));

    light_space.min = vec3{std::numeric_limits<float>::max()};
    light_space.max = vec3{std::numeric_limits<float>::lowest()};

    for (const auto &v : corners) {
        const auto trf = light_space.view * vec4(v, 1.0f);
        light_space.min.x = std::min(light_space.min.x, trf.x);
        light_space.max.x = std::max(light_space.max.x, trf.x);
        light_space.min.y = std::min(light_space.min.y, trf.y);
        light_space.max.y = std::max(light_space.max.y, trf.y);
        light_space.min.z = std::min(light_space.min.z, trf.z);
        light_space.max.z = std::max(light_space.max.z, trf.z);
    }

    constexpr float z_mult = 10.0f;
    if (light_space.min.z < 0)
        light_space.min.z *= z_mult;
    else
        light_space.min.z /= z_mult;

    if (light_space.max.z < 0)
        light_space.max.z /= z_mult;
    else
        light_space.max.z *= z_mult;

    light_space.proj = glm::ortho(light_space.min.x,
                                  light_space.max.x,
                                  light_space.min.y,
                                  light_space.max.y,
                                  light_space.min.z,
                                  light_space.max.z);

    light_space.proj_view = light_space.proj * light_space.view;
}

ShadowMap::ShadowMap()
    : m_light_space_buffer{UNIFORM_BUFFER}
{
    m_shader = DefaultShaders::getShadowMap();
}

void ShadowMap::create(const i32vec2 &size, float near, float far)
{
    destroy();

    m_light_space_buffer.create(sizeof(mat4));

    m_render_texture.create(size);
    m_render_texture.setViewport({0, 0, size.x, size.y});

    m_light_space.near = near;
    m_light_space.far = far;
}

bool ShadowMap::isValid() const
{
    return m_render_texture.isValid();
}

void ShadowMap::destroy()
{
    m_render_texture.destroy();
    m_light_space_buffer.destroy();
}

void ShadowMap::update(const std::shared_ptr<Camera> &camera, const std::shared_ptr<Lights> &lights)
{
    calculateLightSpace(m_light_space, camera, lights);
    m_light_space_buffer.setData(&m_light_space.proj_view, sizeof(mat4));
}

void ShadowMap::applyToSceneShader(const std::shared_ptr<Shader> &scene_shader)
{
    scene_shader->uniformMatrix("u_lightSpaceMat", m_light_space.proj_view);

    glActiveTexture(GL_TEXTURE1);
    scene_shader->uniformSampler("u_shadowMap", 1);
    glBindTexture(GL_TEXTURE_2D, m_render_texture.getTexture().getId());
}

void ShadowMap::saveToFile(const std::string &file_name)
{
    m_render_texture.saveTofile(file_name);
}

void ShadowMap::clear(const vec4 &color) const
{
    m_render_texture.clear(color);

    Shader::use(m_shader.get());
    m_shader->uniformMatrix("u_lightSpaceMat", m_light_space.proj_view);
}

void ShadowMap::display() const
{
    m_render_texture.display();
    Shader::use(nullptr);
}

CascadeShadowMap::CascadeShadowMap()
    : m_light_spaces_buffer{UNIFORM_BUFFER}
{
    m_shader = DefaultShaders::getCasacadeShadowMap();
}

void CascadeShadowMap::create(const i32vec2 &size, const std::vector<float> planes)
{
    destroy();

    if (planes.size() - 2 > CASCADE_SHADOW_MAP_MAX_MATRICES)
        return;

    m_render_texture.create(size, planes.size() - 1);
    m_render_texture.setViewport({0, 0, size.x, size.y});

    m_light_spaces_buffer.create(CASCADE_SHADOW_MAP_MAX_MATRICES * sizeof(mat4)
                                 + CASCADE_SHADOW_MAP_MAX_MATRICES * FLOAT_ARRAY_ITEM_SIZE
                                 + FLOAT_ARRAY_ITEM_SIZE);

    int32_t planes_count = planes.size() - 2;
    m_light_spaces_buffer.setData(&planes_count,
                                  sizeof(int32_t),
                                  CASCADE_SHADOW_MAP_MAX_MATRICES * sizeof(mat4)
                                      + CASCADE_SHADOW_MAP_MAX_MATRICES * FLOAT_ARRAY_ITEM_SIZE);

    for (int32_t i = 0; i < planes.size(); ++i) {
        if (i > 0) {
            m_light_spaces_buffer.setData(&planes[i],
                                          sizeof(float),
                                          CASCADE_SHADOW_MAP_MAX_MATRICES * sizeof(mat4)
                                              + (i - 1) * FLOAT_ARRAY_ITEM_SIZE);
        }

        if (i != (planes.size() - 1)) {
            LightSpace light_space;
            light_space.near = planes[i];
            light_space.far = planes[i + 1];
            m_light_spaces.push_back(light_space);
        }
    }
}

bool CascadeShadowMap::isValid() const
{
    return m_render_texture.isValid();
}

void CascadeShadowMap::destroy()
{
    m_light_spaces_buffer.destroy();
    m_render_texture.destroy();
    m_light_spaces.clear();
}

void CascadeShadowMap::update(const std::shared_ptr<Camera> &camera,
                              const std::shared_ptr<Lights> &lights)
{
    for (int32_t i = 0; i < m_light_spaces.size(); ++i) {
        calculateLightSpace(m_light_spaces[i], camera, lights);
        m_light_spaces_buffer.setData(&m_light_spaces[i].proj_view, sizeof(mat4), i * sizeof(mat4));
    }
}

void CascadeShadowMap::applyToSceneShader(const std::shared_ptr<Shader> &scene_shader)
{
    glActiveTexture(GL_TEXTURE1);
    scene_shader->uniformSampler("u_shadowMap", 1);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_render_texture.getTexture());

    GLBuffer::bindToShader(m_light_spaces_buffer, 1);
}

void CascadeShadowMap::saveToFile(const std::string &file_name)
{
    m_render_texture.saveTofile(file_name);
}

void CascadeShadowMap::clear(const vec4 &color) const
{
    m_render_texture.clear(color);

    Shader::use(m_shader.get());
    GLBuffer::bindToShader(m_light_spaces_buffer, 1);
}

void CascadeShadowMap::display() const
{
    m_render_texture.display();
    Shader::use(nullptr);
}

} // namespace eb
