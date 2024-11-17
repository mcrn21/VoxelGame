#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../3rd/stb_image.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

namespace eb {

Texture::Texture()
    : m_id{0}
    , m_size{0}
    , m_valid{false}
{}

Texture::Texture(uint32_t id, const glm::i32vec2 &size)
    : m_id{id}
    , m_size{size}
    , m_valid{true}
{}

Texture::~Texture()
{
    destroy();
}

glm::i32vec2 Texture::getSize() const
{
    return m_size;
}

glm::vec4 Texture::getUVRect(const glm::i32vec4 &sub_rect) const
{
    if (!m_valid || m_size.x == 0 || m_size.y == 0 || sub_rect.z == 0 || sub_rect.w == 0)
        return glm::vec4{0.0f};

    glm::vec4 uv{0.0f};

    uv.x = static_cast<float>(sub_rect.x) / m_size.x;
    uv.y = static_cast<float>(sub_rect.y) / m_size.y;
    uv.z = static_cast<float>(sub_rect.x + sub_rect.z) / m_size.x;
    uv.w = static_cast<float>(sub_rect.y + sub_rect.w) / m_size.y;

    return uv;
}

bool Texture::loadFromFile(const std::filesystem::path &path)
{
    destroy();

    auto a_path = std::filesystem::absolute(path);
    int32_t width, height, nr_components;

    uint8_t *data = stbi_load(path.c_str(), &width, &height, &nr_components, 0);

    if (!data)
        return false;

    GLenum format;
    if (nr_components == 1)
        format = GL_RED;
    else if (nr_components == 3)
        format = GL_RGB;
    else if (nr_components == 4)
        format = GL_RGBA;

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    m_size = glm::i32vec2{width, height};
    m_valid = true;

    return true;
}

bool Texture::isValid() const
{
    return m_valid;
}

void Texture::destroy()
{
    if (m_valid) {
        m_valid = false;
        glDeleteTextures(1, &m_id);
    }
}

void Texture::bind(const Texture *texture)
{
    if (texture == nullptr) {
        glBindTexture(GL_TEXTURE_2D, 0);
        return;
    }

    if (texture->m_valid)
        glBindTexture(GL_TEXTURE_2D, texture->m_id);
}

} // namespace eb
