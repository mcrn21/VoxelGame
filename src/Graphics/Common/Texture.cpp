#include "Texture.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "../../3rd/stb_image.h"

#include <GL/glew.h>
#include <spdlog/spdlog.h>

namespace eb {

Texture::Texture()
    : m_id{0}
    , m_size{0}
    , m_valid{false}
{}

Texture::Texture(uint32_t id, const i32vec2 &size)
    : m_id{id}
    , m_size{size}
    , m_valid{true}
{}

Texture::~Texture()
{
    destroy();
}

bool Texture::loadFromFile(const std::filesystem::path &path)
{
    if (path.empty())
        return false;

    int32_t width = 0;
    int32_t height = 0;
    int32_t nr_components = 0;

    uint8_t *image_data = stbi_load(path.c_str(), &width, &height, &nr_components, 0);

    if (!image_data)
        return false;

    Format format;
    if (nr_components == 1)
        format = LUMINANCE;
    else if (nr_components == 2)
        format = LUMINANCE_ALPHA;
    else if (nr_components == 3)
        format = RGB;
    else if (nr_components == 4)
        format = RGBA;

    create({width, height}, format, image_data);

    stbi_image_free(image_data);

    return true;
}

bool Texture::loadFromData(const uint8_t *data, int32_t size)
{
    if (data == nullptr || size <= 0)
        return false;

    uint8_t *image_data = nullptr;
    int32_t width = 0;
    int32_t height = 0;
    int32_t nr_components = 0;

    image_data = stbi_load_from_memory(data, size, &width, &height, &nr_components, 0);

    if (!image_data)
        return false;

    Format format;
    if (nr_components == 1)
        format = LUMINANCE;
    else if (nr_components == 2)
        format = LUMINANCE_ALPHA;
    else if (nr_components == 3)
        format = RGB;
    else if (nr_components == 4)
        format = RGBA;

    create({width, height}, format, image_data);

    stbi_image_free(image_data);

    return true;
}

uint32_t Texture::getId() const
{
    return m_id;
}

i32vec2 Texture::getSize() const
{
    return m_size;
}

vec4 Texture::getUVRect(const i32vec4 &sub_rect) const
{
    if (!m_valid || m_size.x == 0 || m_size.y == 0 || sub_rect.z == 0 || sub_rect.w == 0)
        return vec4{0.0f};

    vec4 uv{0.0f};

    uv.x = static_cast<float>(sub_rect.x) / m_size.x;
    uv.y = static_cast<float>(sub_rect.y) / m_size.y;
    uv.z = static_cast<float>(sub_rect.x + sub_rect.z) / m_size.x;
    uv.w = static_cast<float>(sub_rect.y + sub_rect.w) / m_size.y;

    return uv;
}

void Texture::create(const i32vec2 &size, Format format, const uint8_t *data)
{
    destroy();

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, size.x, size.y, 0, format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    m_size = size;
    m_valid = true;
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
