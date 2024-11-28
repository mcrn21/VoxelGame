#ifndef EB_ASSETS_TEXTURELOADER_H
#define EB_ASSETS_TEXTURELOADER_H

#include "../Graphics/Common/Texture.h"
#include "AssetLoader.h"
#include "Assets.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "../3rd/stb_image.h"

#include <GL/glew.h>

#include <filesystem>

namespace eb {

template<>
class AssetLoader<Texture>
{
public:
    static std::shared_ptr<Texture> loadFromFile(Assets *assets,
                                                 const std::string &asset_name,
                                                 const std::filesystem::path &path)
    {
        if (path.empty())
            return nullptr;

        std::string name = asset_name.empty() ? path.stem().string() : asset_name;
        if (name.empty())
            return nullptr;

        int32_t width = 0;
        int32_t height = 0;
        int32_t nr_components = 0;

        uint8_t *data = stbi_load(path.c_str(), &width, &height, &nr_components, 0);

        if (!data)
            return nullptr;

        GLenum format;
        if (nr_components == 1)
            format = GL_LUMINANCE;
        else if (nr_components == 2)
            format = GL_LUMINANCE_ALPHA;
        else if (nr_components == 3)
            format = GL_RGB;
        else if (nr_components == 4)
            format = GL_RGBA;

        GLuint id;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);

        auto texture = std::make_shared<Texture>(id, i32vec2{width, height});
        assets->add(name, texture);
        return texture;
    }
};

} // namespace eb

#endif // EB_ASSETS_TEXTURELOADER_H
