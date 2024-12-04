#ifndef EB_GRAPHICS_DEFAULTSHADERS_H
#define EB_GRAPHICS_DEFAULTSHADERS_H

#include "Shader.h"

#include <memory>

namespace eb {

struct DefaultShaders
{
    static std::shared_ptr<Shader> getMain();
    static std::shared_ptr<Shader> getMainShadowMap();
    static std::shared_ptr<Shader> getMainCascadeShadowMap();
    static std::shared_ptr<Shader> getMain2D();
    static std::shared_ptr<Shader> getShadowMap();
    static std::shared_ptr<Shader> getCasacadeShadowMap();

    static std::shared_ptr<Shader> getVoxels();
    static std::shared_ptr<Shader> getLines();
    static std::shared_ptr<Shader> getMesh();
    static std::shared_ptr<Shader> getShadowmap();
    static std::shared_ptr<Shader> getCascadeShadowmap();
};

} // namespace eb

#endif // EB_GRAPHICS_DEFAULTSHADERS_H
