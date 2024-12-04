#ifndef EB_ASSETS_MODELLOADER_H
#define EB_ASSETS_MODELLOADER_H

#include "../Graphics/Common/Model.h"
#include "AssetLoader.h"
#include "Assets.h"

#include <filesystem>

namespace eb {

template<>
class AssetLoader<Model>
{
public:
    static std::shared_ptr<Model> loadFromFile(Assets *assets,
                                               const std::string &asset_name,
                                               const std::filesystem::path &path)
    {
        if (path.empty())
            return nullptr;

        std::string name = asset_name.empty() ? path.stem().string() : asset_name;
        if (name.empty())
            return nullptr;

        auto model = std::make_shared<Model>();
        if (model->loadFromFile(path)) {
            for (const auto &texture : model->getTextures()) {
                if (!texture.first.empty() && texture.second && !assets->has<Texture>(texture.first))
                    assets->add<Texture>(texture.first, texture.second);
            }
            assets->add<Model>(name, model);
        }

        return model;
    }
};

} // namespace eb

#endif // EB_ASSETS_MODELLOADER_H
