#include "RenderTarget.h"
#include "Drawable.h"

#include <GL/glew.h>
#include <glm/ext.hpp>

namespace eb {

struct Material
{
    glm::vec3 ambient = {1.0f, 1.0f, 1.0f};
    glm::vec3 diffuse = {1.0f, 1.0f, 1.0f};
    glm::vec3 specular = {0.0f, 0.0f, 0.0f};
    float shininess = 32.0f;
};

const Material MATERIAL;

RenderTarget::RenderTarget()
    : m_size{1}
    , m_viewport{1}
    , m_camera{std::make_shared<Camera>()}
    , m_lights{std::make_shared<Lights>()}
    , m_2d_mat{1.0f}
{}

const glm::i32vec2 &RenderTarget::getSize() const
{
    return m_size;
}

void RenderTarget::setSize(const glm::i32vec2 &size)
{
    m_size = size;
    m_2d_mat = glm::ortho(0.0f,
                          static_cast<float>(m_size.x),
                          0.0f,
                          static_cast<float>(m_size.y),
                          -1.0f,
                          1.0f);
}

const glm::i32vec4 &RenderTarget::getViewport() const
{
    return m_viewport;
}

void RenderTarget::setViewport(const glm::i32vec4 &viewport)
{
    m_viewport = viewport;
    glViewport(m_viewport.x, m_viewport.y, m_viewport.z, m_viewport.w);
}

std::shared_ptr<Camera> RenderTarget::getCamera() const
{
    return m_camera;
}

void RenderTarget::setCamera(const std::shared_ptr<Camera> &camera)
{
    if (!camera)
        return;
    m_camera = camera;
}

std::shared_ptr<Lights> RenderTarget::getLights() const
{
    return m_lights;
}

void RenderTarget::setLights(const std::shared_ptr<Lights> &lights)
{
    if (!lights)
        return;
    m_lights = lights;
}

const glm::mat4 &RenderTarget::get2DMat() const
{
    return m_2d_mat;
}

void RenderTarget::setClearColor(const glm::vec4 &color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void RenderTarget::draw(const Drawable &drawable) const
{
    RenderState render_state;
    drawable.draw(*this, render_state);
}

void RenderTarget::draw3D(const VertexArray &vertex_array,
                          const RenderState &render_state,
                          PrimitiveType primitive_type) const
{
    if (render_state.shader) {
        Shader::use(render_state.shader);
        render_state.shader->uniformMatrix("u_model", render_state.transform);
        // render_state.shader->uniformMatrix("u_normals_model", render_state.normals_transform);
        render_state.shader->uniformMatrix("u_view", m_camera->getViewMat());
        render_state.shader->uniformMatrix("u_projection", m_camera->getProjectionMat());

        render_state.shader->uniformVec3("u_camera", m_camera->getPosition());

        render_state.shader->uniformVec3("u_material.ambient", MATERIAL.ambient);
        render_state.shader->uniformVec3("u_material.diffuse", MATERIAL.diffuse);
        render_state.shader->uniformVec3("u_material.specular", MATERIAL.specular);
        render_state.shader->uniformFloat("u_material.shininess", MATERIAL.shininess);

        GlBuffer::bindToShader(m_lights->getLightsSSBO(), 0);
    }

    if (render_state.texture)
        Texture::bind(render_state.texture);

    vertex_array.draw(primitive_type);

    if (render_state.texture)
        Texture::bind(nullptr);

    if (render_state.shader)
        Shader::use(nullptr);
}

void RenderTarget::draw2D(const VertexArray &vertex_array,
                          const RenderState &render_state,
                          PrimitiveType primitive_type) const
{
    if (render_state.shader) {
        Shader::use(render_state.shader);
        render_state.shader->uniformMatrix("u_model", render_state.transform);
        render_state.shader->uniformMatrix("u_projection", m_2d_mat);
    }

    if (render_state.texture)
        Texture::bind(render_state.texture);

    vertex_array.draw(primitive_type);

    if (render_state.texture)
        Texture::bind(nullptr);

    if (render_state.shader)
        Shader::use(nullptr);
}

} // namespace eb
