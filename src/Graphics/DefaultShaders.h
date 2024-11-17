#ifndef EB_GRAPHICS_DEFAULTSHADERS_H
#define EB_GRAPHICS_DEFAULTSHADERS_H

#include "Shader.h"

#include <memory>

namespace eb {

struct DefaultShaders
{
    static std::shared_ptr<Shader> getMain();
    static std::shared_ptr<Shader> getMain2D();
    static std::shared_ptr<Shader> getVoxels();
    static std::shared_ptr<Shader> getLines();
};

} // namespace eb

#endif // EB_GRAPHICS_DEFAULTSHADERS_H
