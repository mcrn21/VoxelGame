#ifndef EB_GRAPHICS_TEXTURE_H
#define EB_GRAPHICS_TEXTURE_H

#include <glm/glm.hpp>

#include <filesystem>

namespace eb {

class TextureLoader;

class Texture
{
    friend class TextureLoader;

public:
    enum Type { DIFFUSE, SPECULAR, HEIGHT, AMBIENT };

    Texture();
    Texture(uint32_t id, const glm::i32vec2 &size);
    ~Texture();

    glm::i32vec2 getSize() const;
    glm::vec4 getUVRect(const glm::i32vec4 &sub_rect) const;

    Type getType() const;
    void setType(Type type);

    bool loadFromFile(const std::filesystem::path &path);
    bool isValid() const;
    void destroy();

    static void bind(const Texture *texture);

private:
    uint32_t m_id;
    glm::i32vec2 m_size;
    bool m_valid;
    Type m_type;
};

} // namespace eb

#endif // EB_GRAPHICS_TEXTURE_H
