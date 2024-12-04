#include "DepthRenderTextureArray.h"

#include "../../3rd/stb_image_write.h"

#include <GL/glew.h>

#include <vector>

namespace eb {

DepthRenderTextureArray::DepthRenderTextureArray()
    : RenderTarget{}
    , m_texture{0}
    , m_fbo{0}
    , m_valid{false}
{}

DepthRenderTextureArray::~DepthRenderTextureArray()
{
    destroy();
}

uint32_t DepthRenderTextureArray::getTexture() const
{
    return m_texture;
}

void DepthRenderTextureArray::create(const i32vec2 &size, int32_t layers)
{
    glGenFramebuffers(1, &m_fbo);

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
    glTexImage3D(GL_TEXTURE_2D_ARRAY,
                 0,
                 GL_DEPTH_COMPONENT32F,
                 size.x,
                 size.y,
                 layers,
                 0,
                 GL_DEPTH_COMPONENT,
                 GL_FLOAT,
                 nullptr);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr float bordercolor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    RenderTarget::setSize(size);
    m_layers = layers;
    m_valid = true;
}

bool DepthRenderTextureArray::isValid() const
{
    return m_valid;
}

void DepthRenderTextureArray::destroy()
{
    if (!m_valid)
        return;

    glDeleteTextures(1, &m_texture);
    glDeleteFramebuffers(1, &m_fbo);

    m_texture = 0;
    m_fbo = 0;
    m_layers = 0;
    m_valid = false;
}

void DepthRenderTextureArray::clear(const vec4 &color) const
{
    if (!m_valid)
        return;

    applyViewport();
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    applyClearColor(color);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void DepthRenderTextureArray::display() const
{
    if (!m_valid)
        return;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthRenderTextureArray::saveTofile(const std::string &file_name) const
{
    if (!m_valid)
        return;

    int32_t size = getSize().x * getSize().y;

    if (size == 0)
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    for (int32_t i = 0; i < m_layers; ++i) {
        std::vector<float> data;
        data.resize(size, 0);

        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_texture, 0, i);
        glReadPixels(0, 0, getSize().x, getSize().y, GL_DEPTH_COMPONENT, GL_FLOAT, data.data());

        std::vector<uint8_t> out_data;

        for (float p : data) {
            out_data.push_back((255) * p);
            out_data.push_back((255) * p);
            out_data.push_back((255) * p);
        }

        int32_t dot_pos = file_name.find('.');
        if (dot_pos == -1)
            dot_pos = file_name.size();
        std::string fn = file_name;
        fn.insert(dot_pos, "_" + std::to_string(i));

        stbi_write_png(fn.c_str(), getSize().x, getSize().y, 3, out_data.data(), 0);
    }

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return;
}

} // namespace eb
