#ifndef EB_GRAPHICS_SHADER_H
#define EB_GRAPHICS_SHADER_H

#include <glm/glm.hpp>

#include <filesystem>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace eb {

class GLBuffer;
class Material;

class Shader
{
public:
    Shader();
    Shader(uint32_t id);
    Shader(const std::filesystem::path &vertex_path,
           const std::filesystem::path &geometry_path,
           const std::filesystem::path &fragment_path);
    Shader(const std::string &vertex_string,
           const std::string &geometry_string,
           const std::string &fragment_string);
    ~Shader();

    uint32_t getId() const;

    bool loadFromFile(const std::filesystem::path &vertex_path,
                      const std::filesystem::path &geometry_path,
                      const std::filesystem::path &fragment_path);
    bool loadFromString(const std::string &vertex_string,
                        const std::string &geometry_string,
                        const std::string &fragment_string);
    bool isValid() const;
    void destroy();

    void uniformMatrix(const std::string &name, const glm::mat4 &matrix) const;
    void uniformVec3(const std::string &name, const glm::vec3 &vec) const;
    void uniformFloat(const std::string &name, float value) const;
    void uniformInt(const std::string &name, int32_t value) const;
    void uniformBuffer();
    void uniformSampler(const std::string &name, int32_t num) const;

    // Spcial uniforms
    // Set u_material and bind texture
    void uniformMaterial(const Material &material) const;

    static void use(const Shader *shader);

private:
    uint32_t getUniformLocation(const std::string &name) const;
    std::pair<uint32_t, bool> createShaderFromString(const std::string &shader_str,
                                                     int32_t type) const;
    std::pair<uint32_t, bool> createProgramm(const std::vector<uint32_t> &shaders) const;

private:
    uint32_t m_id;
    bool m_valid;
    mutable std::unordered_map<std::string, uint32_t> m_uniform_locations;
};

} // namespace eb

#endif // EB_GRAPHICS_SHADER_H
