#include "Scene2D.h"
#include "Graphics/Common/DefaultShaders.h"

#include <glm/ext.hpp>

namespace eb {

Scene2D::Scene2D()
    : m_projection_mat{1.0f}
{}

void Scene2D::updateProjectionMatrix(const i32vec2 &size)
{
    m_projection_mat = glm::ortho(0.0f,
                                  static_cast<float>(size.x),
                                  0.0f,
                                  static_cast<float>(size.y),
                                  -1.0f,
                                  1.0f);
}

void Scene2D::draw(const Drawable &drawable) const
{
    RenderState render_state;
    render_state.shader = DefaultShaders::getMain2D().get();

    Shader::use(render_state.shader);
    render_state.shader->uniformMatrix("u_projection", m_projection_mat);

    drawable.draw(render_state);
}

} // namespace eb
