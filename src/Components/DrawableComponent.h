#ifndef EB_COMPONENTS_DRAWABLECOMPONENT_H
#define EB_COMPONENTS_DRAWABLECOMPONENT_H

#include "../Graphics/Common/Drawable.h"

#include <memory>

namespace eb {

struct DrawableComponent
{
    std::shared_ptr<Drawable> drawable;
};

} // namespace eb

#endif // EB_COMPONENTS_DRAWABLECOMPONENT_H
