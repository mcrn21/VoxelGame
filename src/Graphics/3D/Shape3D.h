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
    void createPlane(const vec3 &p1, const vec3 &p2, const vec3 &p3, const vec3 &p4);
};

} // namespace eb

#endif // EB_GRAPHICS_SHAPE3D_H
