#ifndef EB_ASSETS_ASSETLOADER_H
#define EB_ASSETS_ASSETLOADER_H

#include <memory>

namespace eb {

class Assets;

template<typename T>
class AssetLoader
{
public:
    template<typename... Args>
    static std::shared_ptr<T> loadFromFile(Assets *, const std::string &, Args &&...args)
    {
        return nullptr;
    }

    template<typename... Args>
    static std::shared_ptr<T> loadFromData(Assets *, const std::string &, Args &&...args)
    {
        return nullptr;
    }
};

} // namespace eb

#endif // EB_ASSETS_ASSETLOADER_H
