#ifndef EB_GRAPHICS_3D_LINESBATCH_H
#define EB_GRAPHICS_3D_LINESBATCH_H

#include "VertexArrayInstance.h"

using namespace glm;

namespace eb {

class LinesBatch : public VertexArrayInstance
{
public:
    LinesBatch();
    ~LinesBatch() = default;

    float getLineWidth() const;
    void setLineWidth(float line_width);

    void cube(const vec3 &from, const vec3 &to, const vec4 &color);

    void draw(const RenderState &render_state) const;

private:
    float m_line_width;
};

} // namespace eb

#endif // EB_GRAPHICS_3D_LINESBATCH_H
