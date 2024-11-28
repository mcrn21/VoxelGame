#ifndef EB_SCENE2D_H
#define EB_SCENE2D_H

#include "Graphics/Common/Drawable.h"

#include <glm/glm.hpp>

using namespace glm;

namespace eb {

class Scene2D
{
public:
    Scene2D();
    ~Scene2D() = default;

    void updateProjectionMatrix(const i32vec2 &size);

    void draw(const Drawable &drawable) const;

private:
    mat4 m_projection_mat;
};

} // namespace eb

#endif // EB_SCENE2D_H
