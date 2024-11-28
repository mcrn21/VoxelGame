#ifndef EB_GRAPHICS_SHAPE3D_H
#define EB_GRAPHICS_SHAPE3D_H

#include "VertexArrayInstance.h"

namespace eb {

class Shape3D : public VertexArrayInstance
{
public:
    Shape3D();
    ~Shape3D() = default;

    void createCube(const vec3 &from, const vec3 &to);
};

} // namespace eb

#endif // EB_GRAPHICS_SHAPE3D_H
