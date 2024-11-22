#ifndef EB_GRAPHICS_MODEL_H
#define EB_GRAPHICS_MODEL_H

#include "Drawable.h"
#include "Mesh.h"
#include "Transformable.h"

#include <filesystem>

#include <memory>

namespace eb {

class ModelLoader;

class Model : public Drawable, public Transformable
{
    friend class ModelLoader;

public:
    Model();
    ~Model() = default;

    bool loadFromFile(const std::filesystem::path &path);

    void draw(const RenderTarget &render_target, const RenderState &render_state) const;

private:
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::vector<std::shared_ptr<Texture>> m_textures;
};

} // namespace eb

#endif // EB_GRAPHICS_MODEL_H
