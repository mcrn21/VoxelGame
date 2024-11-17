#ifndef EB_GRAPHICS_SHADER_H
#define EB_GRAPHICS_SHADER_H

#include <glm/glm.hpp>

#include <filesystem>
#include <stdint.h>
#include <string>
#include <vector>

namespace eb {

class Shader
{
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

    void uniformMatrix(const std::string &name, glm::mat4 matrix);

    static void use(const Shader *shader);

private:
    std::pair<uint32_t, bool> createShaderFromString(const std::string &shader_str, int32_t type);
    std::pair<uint32_t, bool> createProgramm(const std::vector<uint32_t> &shaders);

private:
    uint32_t m_id;
    bool m_valid;
};

} // namespace eb

#endif // EB_GRAPHICS_SHADER_H
