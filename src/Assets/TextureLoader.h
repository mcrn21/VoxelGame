#ifndef EB_ASSETS_TEXTURELOADER_H
#define EB_ASSETS_TEXTURELOADER_H

#include "../Graphics/Common/Texture.h"
#include "AssetLoader.h"
#include "Assets.h"

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

        auto texture = std::make_shared<Texture>();
        if (!texture->loadFromFile(path))
            return nullptr;

        assets->add(name, texture);

        return texture;
    }
};

} // namespace eb

#endif // EB_ASSETS_TEXTURELOADER_H
