#include "Scene3D.h"
#include "Components/DrawableComponent.h"
#include "Graphics/Common/DefaultShaders.h"
#include "Graphics/Common/RenderState.h"

#include "Engine.h"

#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <spdlog/spdlog.h>

#define SHADOW_MAP_SIZE 1024

namespace eb {

Scene3D::Scene3D(Engine *engine)
    : EngineObject{engine}
    , m_camera{std::make_shared<Camera>()}
    , m_lights{std::make_shared<Lights>()}
{
    // Setup shaders
    setMainShader(DefaultShaders::getMainCascadeShadowMap());
    setCSMShader(DefaultShaders::getCasacadeShadowMap());

    m_cascade_shadow_map.create({SHADOW_MAP_SIZE, SHADOW_MAP_SIZE},
                                {0.04, 0.1, 0.2, 0.5},
                                m_camera,
                                m_lights);

    setShadowMap(SIMPLE_SHADOW_MAP, {SHADOW_MAP_SIZE, SHADOW_MAP_SIZE});
}

std::shared_ptr<Camera> Scene3D::getCamera() const
{
    return m_camera;
}

void Scene3D::setCamera(const std::shared_ptr<Camera> &camera)
{
    m_camera = camera;
}

std::shared_ptr<Lights> Scene3D::getLights() const
{
    return m_lights;
}

void Scene3D::setLights(const std::shared_ptr<Lights> &lights)
{
    m_lights = lights;
}

entt::registry &Scene3D::getRegistry()
{
    return m_registry;
}

void Scene3D::setShadowMap(ShadowMapType shadow_map_type,
                           const i32vec2 &size,
                           const std::vector<float> &planes)
{
    m_shadow_map_type = shadow_map_type;
    switch (shadow_map_type) {
    case NO_SHADOWS:
        m_shadow_map.reset();
        m_main_shader = DefaultShaders::getMain();
        break;
    case SIMPLE_SHADOW_MAP: {
        m_shadow_map.reset();
        auto shadow_map = std::make_unique<ShadowMap>();
        shadow_map->create(size, m_camera->getNear(), m_camera->getFar() * 0.3f);
        shadow_map->update(m_camera, m_lights);
        m_shadow_map = std::move(shadow_map);
        m_main_shader = DefaultShaders::getMainShadowMap();

        break;
    }
    case CASCADE_SHADOW_MAP: {
        if (planes.empty()) {
            m_shadow_map.reset();
            m_main_shader = DefaultShaders::getMain();
            break;
        }

        auto shadow_map = std::make_unique<CascadeShadowMap>();
        shadow_map->create(size, planes);
        shadow_map->update(m_camera, m_lights);
        m_shadow_map = std::move(shadow_map);
        m_main_shader = DefaultShaders::getMainCascadeShadowMap();

        break;
    }
    }
}

std::shared_ptr<Shader> Scene3D::getMainShader() const
{
    return m_main_shader;
}

void Scene3D::setMainShader(const std::shared_ptr<Shader> &shader)
{
    m_main_shader = shader;
}

std::shared_ptr<Shader> Scene3D::getCSMShader() const
{
    return m_csm_shader;
}

void Scene3D::setCSMShader(const std::shared_ptr<Shader> &shader)
{
    m_csm_shader = shader;
}

void Scene3D::updateProjectionMatrix(const i32vec2 &size)
{
    m_camera->setRatio(static_cast<float>(size.x) / size.y);
}

void Scene3D::update(const Time &elapsed)
{
    m_camera->resetChangeFlag();
}

void Scene3D::updateTick(const Time &elapsed) {}

void Scene3D::draw(const Time &elapsed, const RenderTarget &render_target) const
{
    RenderState render_state;
    auto view = m_registry.view<DrawableComponent>();

    // glCullFace(GL_FRONT);

    // m_cascade_shadow_map.getRenderTexture().clear();

    // render_state.shader = m_csm_shader.get();

    // Shader::use(render_state.shader);

    // m_cascade_shadow_map.update();

    // GLBuffer::bindToShader(m_cascade_shadow_map.getShadowDataBuffer(), 1);

    // view.each([this, &render_state](const auto &drawable) {
    //     if (drawable.drawable)
    //         drawable.drawable->draw(render_state);
    // });

    // m_cascade_shadow_map.getRenderTexture().display();

    // glCullFace(GL_BACK);

    if (getEngine()->getKeyboard().isKeyJustPressed(GLFW_KEY_4)) {
        const_cast<Scene3D *>(this)->setShadowMap(NO_SHADOWS);
    }

    if (getEngine()->getKeyboard().isKeyJustPressed(GLFW_KEY_5)) {
        const_cast<Scene3D *>(this)->setShadowMap(SIMPLE_SHADOW_MAP,
                                                  {SHADOW_MAP_SIZE, SHADOW_MAP_SIZE});
    }

    if (getEngine()->getKeyboard().isKeyJustPressed(GLFW_KEY_6)) {
        const_cast<Scene3D *>(this)->setShadowMap(CASCADE_SHADOW_MAP,
                                                  {SHADOW_MAP_SIZE, SHADOW_MAP_SIZE},
                                                  {m_camera->getNear(),
                                                   m_camera->getFar() / 10.0f,
                                                   m_camera->getFar() / 5.f,
                                                   m_camera->getFar()});
    }

    // =========== Render shadow map ============
    if (m_shadow_map_type != NO_SHADOWS) {
        if (m_camera->isChanged())
            m_shadow_map->update(m_camera, m_lights);
    }

    if (m_shadow_map_type != NO_SHADOWS) {
        glCullFace(GL_FRONT);

        m_shadow_map->clear();

        render_state.shader = m_shadow_map->getShader().get();

        view.each([this, &render_state](const auto &drawable) {
            if (drawable.drawable)
                drawable.drawable->draw(render_state);
        });

        m_shadow_map->display();

        glCullFace(GL_BACK);
    }

    if (getEngine()->getKeyboard().isKeyJustPressed(GLFW_KEY_3))
        m_shadow_map->saveToFile("depth.png");

    // ==========================================

    // Render main scene
    render_target.clear(glm::vec4{0.45f, 0.72f, 1.0f, 1.0f});

    render_state.shader = m_main_shader.get();

    Shader::use(render_state.shader);

    render_state.shader->uniformMatrix("u_cameraProjMat", m_camera->getProjMat());
    render_state.shader->uniformMatrix("u_cameraViewMat", m_camera->getViewMat());
    render_state.shader->uniformVec3("u_cameraPos", m_camera->getPosition());

    if (m_shadow_map_type != NO_SHADOWS)
        m_shadow_map->applyToSceneShader(m_main_shader);

    // render_state.shader->uniformMatrix("u_cameraViewMat", m_camera->getViewMat());
    // render_state.shader->uniformFloat("u_cameraFar", m_camera->getFar());

    // glActiveTexture(GL_TEXTURE1);
    // render_state.shader->uniformSampler("u_shadowMap", 1);
    // glBindTexture(GL_TEXTURE_2D_ARRAY, m_cascade_shadow_map.getRenderTexture().getTexture());
    // GLBuffer::bindToShader(m_cascade_shadow_map.getShadowDataBuffer(), 1);

    GLBuffer::bindToShader(m_lights->getLightsSSBO(), 0);

    view.each([this, &render_state](const auto &drawable) {
        if (drawable.drawable)
            drawable.drawable->draw(render_state);
    });
}

} // namespace eb
