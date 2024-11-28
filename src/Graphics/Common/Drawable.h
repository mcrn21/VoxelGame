#ifndef EB_GRAPHICS_DRAWABLE_H
#define EB_GRAPHICS_DRAWABLE_H

#include "RenderState.h"
#include "Transformable.h"

namespace eb {

class Drawable : public Transformable
{
public:
    Drawable()
        : Transformable{} {};
    virtual ~Drawable() = default;

    virtual void draw(const RenderState &) const = 0;
};

} // namespace eb

#endif // EB_GRAPHICS_DRAWABLE_H
