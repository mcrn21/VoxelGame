#include "Scene3D.h"
#include "Components/DrawableComponent.h"
#include "Graphics/Common/DefaultShaders.h"
#include "Graphics/Common/Drawable.h"
#include "Graphics/Common/RenderState.h"

namespace eb {

Scene3D::Scene3D()
    : m_camera{std::make_shared<Camera>()}
    , m_lights{std::make_shared<Lights>()}
{}

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

void Scene3D::updateProjectionMatrix(const i32vec2 &size)
{
    m_camera->setRatio(static_cast<float>(size.x) / size.y);
}

void Scene3D::update(const Time &elapsed) {}

void Scene3D::draw(const Time &elapsed, const RenderTarget &render_target) const
{
    // Render main scene
    RenderState render_state;
    render_state.shader = DefaultShaders::getMesh().get();

    Shader::use(render_state.shader);
    render_state.shader->uniformMatrix("u_view", m_camera->getViewMat());
    render_state.shader->uniformMatrix("u_projection", m_camera->getProjectionMat());
    render_state.shader->uniformVec3("u_camera", m_camera->getPosition());

    GlBuffer::bindToShader(m_lights->getLightsSSBO(), 0);

    auto view = m_registry.view<DrawableComponent>();
    view.each([this, &render_state](const auto &drawable) {
        if (drawable.drawable)
            drawable.drawable->draw(render_state);
    });
}

} // namespace eb
