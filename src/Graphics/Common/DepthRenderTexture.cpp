#include "DepthRenderTexture.h"

#include "../../3rd/stb_image_write.h"

#include <vector>

namespace eb {

DepthRenderTexture::DepthRenderTexture()
    : RenderTarget{}
    , m_fbo{0}
    , m_valid{false}
{}

DepthRenderTexture::~DepthRenderTexture()
{
    destroy();
}

const Texture &DepthRenderTexture::getTexture() const
{
    return m_texture;
}

void DepthRenderTexture::create(const i32vec2 &size)
{
    glGenFramebuffers(1, &m_fbo);

    uint32_t m_texture_id;
    glGenTextures(1, &m_texture_id);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_DEPTH_COMPONENT,
                 size.x,
                 size.y,
                 0,
                 GL_DEPTH_COMPONENT,
                 GL_FLOAT,
                 NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texture_id, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_texture.m_id = m_texture_id;
    m_texture.m_size = size;
    m_texture.m_valid = true;

    RenderTarget::setSize(size);
    m_valid = true;
}

bool DepthRenderTexture::isValid() const
{
    return m_valid;
}

void DepthRenderTexture::destroy()
{
    if (!m_valid)
        return;
    m_texture.destroy();

    glDeleteFramebuffers(1, &m_fbo);

    m_fbo = 0;
    m_valid = false;
}

void DepthRenderTexture::clear(const vec4 &color) const
{
    if (!m_valid)
        return;

    applyViewport();
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    applyClearColor(color);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void DepthRenderTexture::display() const
{
    if (!m_valid)
        return;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthRenderTexture::saveTofile(const std::string &file_name) const
{
    if (!m_valid)
        return;

    int32_t size = getSize().x * getSize().y;

    if (size == 0)
        return;

    std::vector<float> data;
    data.resize(size, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glReadPixels(0, 0, getSize().x, getSize().y, GL_DEPTH_COMPONENT, GL_FLOAT, data.data());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::vector<uint8_t> out_data;

    for (float p : data) {
        out_data.push_back((255) * p);
        out_data.push_back((255) * p);
        out_data.push_back((255) * p);
    }

    stbi_write_png(file_name.c_str(), getSize().x, getSize().y, 3, out_data.data(), 0);

    return;
}

} // namespace eb
