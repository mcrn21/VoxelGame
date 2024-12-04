#ifndef EB_ASSETS_SHADERLOADER_H
#define EB_ASSETS_SHADERLOADER_H

#include "../Graphics/Common/Shader.h"
#include "AssetLoader.h"
#include "Assets.h"

namespace eb {

template<>
class AssetLoader<Shader>
{
public:
    static std::shared_ptr<Shader> loadFromFile(Assets *assets,
                                                const std::string &asset_name,
                                                const std::filesystem::path &vertex_path,
                                                const std::filesystem::path &geometry_path,
                                                const std::filesystem::path &fragment_path)
    {
        if (asset_name.empty())
            return nullptr;

        auto shader = std::make_shared<Shader>();
        if (shader->loadFromFile(vertex_path, geometry_path, fragment_path))
            assets->add<Shader>(asset_name, shader);

        return shader;
    }
};

} // namespace eb

#endif // EB_ASSETS_SHADERLOADER_H
