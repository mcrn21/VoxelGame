#ifndef EB_GRAPHICS_DRAWABLE_H
#define EB_GRAPHICS_DRAWABLE_H

#include "../Window/Window.h"
#include "RenderTarget.h"

namespace eb {

class Drawable
{
public:
    Drawable() = default;
    virtual ~Drawable() = default;

    virtual void draw(const RenderTarget &) const = 0;
};

} // namespace eb

#endif // EB_GRAPHICS_DRAWABLE_H
