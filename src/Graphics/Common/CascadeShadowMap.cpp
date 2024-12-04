#include "CascadeShadowMap.h"

#include <glm/ext.hpp>

#define MAX_CASCADES 15
#define MAX_MATRICES 16
#define FLOAT_ARRAY_ITEM_SIZE 16

namespace eb {

CascadeShadowMap2::CascadeShadowMap2()
    : m_shadow_data_buffer{UNIFORM_BUFFER}
{}

CascadeShadowMap2::~CascadeShadowMap2()
{
    destroy();
}

void CascadeShadowMap2::create(const vec2 &size,
                               const std::vector<float> &cascade_levels,
                               const std::shared_ptr<Camera> &camera,
                               const std::shared_ptr<Lights> &lights)
{
    if (cascade_levels.size() > MAX_CASCADES)
        return;

    destroy();

    m_camera = camera;
    m_lights = lights;

    m_render_texture.create(size, cascade_levels.size() + 1);
    m_render_texture.setViewport({0, 0, size.x, size.y});

    m_cascade_levels = cascade_levels;

    int32_t cascade_levels_count = cascade_levels.size();

    m_shadow_data_buffer.create(MAX_MATRICES * sizeof(mat4) + MAX_MATRICES * FLOAT_ARRAY_ITEM_SIZE
                                + FLOAT_ARRAY_ITEM_SIZE);
    m_shadow_data_buffer.setData(&cascade_levels_count,
                                 sizeof(int32_t),
                                 MAX_MATRICES * sizeof(mat4) + MAX_MATRICES * FLOAT_ARRAY_ITEM_SIZE);

    for (int32_t i = 0; i < cascade_levels_count; ++i) {
        float level = m_camera->getFar() * cascade_levels[i];
        m_shadow_data_buffer.setData(&level,
                                     sizeof(float),
                                     MAX_MATRICES * sizeof(mat4) + i * FLOAT_ARRAY_ITEM_SIZE);
    }

    m_cascade_cache.clear();

    for (int32_t i = 0; i < cascade_levels.size() + 1; ++i) {
        CascadeCache cascade_cache;

        if (i == 0) {
            cascade_cache.near = m_camera->getNear();
            cascade_cache.far = m_camera->getFar() * cascade_levels[i];
        } else if (i < cascade_levels.size()) {
            cascade_cache.near = m_camera->getFar() * cascade_levels[i - 1];
            cascade_cache.far = m_camera->getFar() * cascade_levels[i];
        } else {
            cascade_cache.near = m_camera->getFar() * cascade_levels[i - 1];
            cascade_cache.far = m_camera->getFar();
        }

        m_cascade_cache.push_back(cascade_cache);
    }

    update();
}

void CascadeShadowMap2::destroy()
{
    m_shadow_data_buffer.destroy();
    m_render_texture.destroy();
}

const DepthRenderTextureArray &CascadeShadowMap2::getRenderTexture() const
{
    return m_render_texture;
}

const GLBuffer &CascadeShadowMap2::getShadowDataBuffer() const
{
    return m_shadow_data_buffer;
}

void CascadeShadowMap2::update()
{
    for (int32_t i = 0; i < m_cascade_cache.size(); ++i) {
        const auto proj = glm::perspective(glm::radians(m_camera->getFov()),
                                           m_camera->getRatio(),
                                           m_cascade_cache[i].near,
                                           m_cascade_cache[i].far);

        m_cascade_cache[i].frustum.update(proj * m_camera->getViewMat());
        const auto &corners = m_cascade_cache[i].frustum.getPoints();

        m_cascade_cache[i].center = vec3{0.0f, 0.0f, 0.0f};
        for (const auto &v : corners) {
            m_cascade_cache[i].center += glm::vec3(v);
        }
        m_cascade_cache[i].center /= corners.size();

        m_cascade_cache[i].view = glm::lookAt(m_cascade_cache[i].center
                                                  - glm::normalize(
                                                      m_lights->getWorldLightDirection()),
                                              m_cascade_cache[i].center,
                                              glm::vec3(0.0f, 1.0f, 0.0f));

        m_cascade_cache[i].min = vec3{std::numeric_limits<float>::max()};
        m_cascade_cache[i].max = vec3{std::numeric_limits<float>::lowest()};

        for (const auto &v : m_cascade_cache[i].frustum.getPoints()) {
            const auto trf = m_cascade_cache[i].view * vec4(v, 1.0f);
            m_cascade_cache[i].min.x = std::min(m_cascade_cache[i].min.x, trf.x);
            m_cascade_cache[i].max.x = std::max(m_cascade_cache[i].max.x, trf.x);
            m_cascade_cache[i].min.y = std::min(m_cascade_cache[i].min.y, trf.y);
            m_cascade_cache[i].max.y = std::max(m_cascade_cache[i].max.y, trf.y);
            m_cascade_cache[i].min.z = std::min(m_cascade_cache[i].min.z, trf.z);
            m_cascade_cache[i].max.z = std::max(m_cascade_cache[i].max.z, trf.z);
        }

        constexpr float z_mult = 10.0f;
        if (m_cascade_cache[i].min.z < 0) {
            m_cascade_cache[i].min.z *= z_mult;
        } else {
            m_cascade_cache[i].min.z /= z_mult;
        }
        if (m_cascade_cache[i].max.z < 0) {
            m_cascade_cache[i].max.z /= z_mult;
        } else {
            m_cascade_cache[i].max.z *= z_mult;
        }

        m_cascade_cache[i].proj = glm::ortho(m_cascade_cache[i].min.x,
                                             m_cascade_cache[i].max.x,
                                             m_cascade_cache[i].min.y,
                                             m_cascade_cache[i].max.y,
                                             m_cascade_cache[i].min.z,
                                             m_cascade_cache[i].max.z);

        m_cascade_cache[i].proj_view = m_cascade_cache[i].proj * m_cascade_cache[i].view;
        m_shadow_data_buffer.setData(&m_cascade_cache[i].proj_view, sizeof(mat4), i * sizeof(mat4));
    }
}

} // namespace eb
