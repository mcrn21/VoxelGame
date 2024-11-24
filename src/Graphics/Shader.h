#ifndef EB_GRAPHICS_SHADER_H
#define EB_GRAPHICS_SHADER_H

#include <glm/glm.hpp>

#include <filesystem>
#include <stdint.h>
#include <string>

namespace eb {

class ShaderLoader;

class Shader
{
    friend class ShaderLoader;

public:
    Shader();
    Shader(uint32_t id);
    Shader(const std::filesystem::path &vertex_path, const std::filesystem::path &fragment_path);
    Shader(const std::string &vertex_string, const std::string &fragment_string);
    ~Shader();

    uint32_t getId() const;

    bool loadFromFile(const std::filesystem::path &vertex_path,
                      const std::filesystem::path &fragment_path);
    bool loadFromString(const std::string &vertex_string, const std::string &fragment_string);
    bool isValid() const;
    void destroy();

    void uniformMatrix(const std::string &name, const glm::mat4 &matrix);
    void uniformVec3(const std::string &name, const glm::vec3 &vec);
    void uniformFloat(const std::string &name, float value);

    static void use(const Shader *shader);

private:
    uint32_t m_id;
    bool m_valid;
};

} // namespace eb

#endif // EB_GRAPHICS_SHADER_H
