#include "Shader.h"
#include "../../Utils/Files.h"
#include "../3D/Material.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <glm/gtc/type_ptr.hpp>

namespace eb {

Shader::Shader()
    : m_id{0}
    , m_valid{false}
{}

Shader::Shader(uint32_t id)
    : m_id{id}
    , m_valid{true}
{}

Shader::Shader(const std::filesystem::path &vertex_path,
               const std::filesystem::path &geometry_path,
               const std::filesystem::path &fragment_path)
    : m_id{0}
    , m_valid{false}
{
    loadFromFile(vertex_path, geometry_path, fragment_path);
}

Shader::Shader(const std::string &vertex_string,
               const std::string &geometry_string,
               const std::string &fragment_string)
    : m_id{0}
    , m_valid{false}
{
    loadFromString(vertex_string, geometry_string, fragment_string);
}

Shader::~Shader()
{
    destroy();
}

uint32_t Shader::getId() const
{
    return m_id;
}

bool Shader::loadFromFile(const std::filesystem::path &vertex_path,
                          const std::filesystem::path &geometry_path,
                          const std::filesystem::path &fragment_path)
{
    try {
        std::string vertex_string;
        std::string geometry_string;
        std::string fragment_string;

        if (!vertex_path.empty())
            vertex_string = Files::readFileToString(vertex_path);
        if (!geometry_path.empty())
            geometry_string = Files::readFileToString(geometry_path);
        if (!fragment_path.empty())
            fragment_string = Files::readFileToString(fragment_path);

        return loadFromString(vertex_string, geometry_string, fragment_string);

    } catch (const std::exception &e) {
        spdlog::error("Error loading shader: {}", e.what());
        return false;
    }
}

bool Shader::loadFromString(const std::string &vertex_string,
                            const std::string &geometry_string,
                            const std::string &fragment_string)
{
    destroy();

    std::vector<uint32_t> shaders;

    if (!vertex_string.empty()) {
        auto vertex_result = createShaderFromString(vertex_string, GL_VERTEX_SHADER);
        if (!vertex_result.second) {
            spdlog::error("Error in shader: {}", vertex_string);
            return false;
        }
        shaders.push_back(vertex_result.first);
    }

    if (!geometry_string.empty()) {
        auto geometry_result = createShaderFromString(geometry_string, GL_GEOMETRY_SHADER);
        if (!geometry_result.second) {
            spdlog::error("Error in shader: {}", fragment_string);
            return false;
        }
        shaders.push_back(geometry_result.first);
    }

    if (!fragment_string.empty()) {
        auto fragment_result = createShaderFromString(fragment_string, GL_FRAGMENT_SHADER);
        if (!fragment_result.second) {
            spdlog::error("Error in shader: {}", fragment_string);
            return false;
        }
        shaders.push_back(fragment_result.first);
    }

    auto programm_result = createProgramm(shaders);

    if (programm_result.second) {
        m_valid = true;
        m_id = programm_result.first;
    }

    for (auto shader : shaders)
        glDeleteShader(shader);

    return true;
}

bool Shader::isValid() const
{
    return m_valid;
}

void Shader::destroy()
{
    if (m_valid) {
        m_valid = false;
        glDeleteProgram(m_id);
    }
}

void Shader::uniformMatrix(const std::string &name, const glm::mat4 &matrix) const
{
    GLuint loc = getUniformLocation(name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::uniformVec3(const std::string &name, const glm::vec3 &vec) const
{
    GLuint loc = getUniformLocation(name);
    glUniform3f(loc, vec.x, vec.y, vec.z);
}

void Shader::uniformFloat(const std::string &name, float value) const
{
    GLuint loc = getUniformLocation(name);
    glUniform1f(loc, value);
}

void Shader::uniformInt(const std::string &name, int32_t value) const
{
    GLuint loc = getUniformLocation(name);
    glUniform1i(loc, value);
}

void Shader::uniformSampler(const std::string &name, int32_t num) const
{
    GLuint loc = getUniformLocation(name);
    glUniform1i(loc, num);
}

void Shader::uniformMaterial(const Material &material) const
{
    if (material.diffuse_texture0) {
        glActiveTexture(GL_TEXTURE0);
        uniformSampler("u_material.diffuse_texture0", 0);
        Texture::bind(material.diffuse_texture0.get());
    }

    uniformVec3("u_material.color", material.color);
    uniformVec3("u_material.specular", material.specular);
    uniformFloat("u_material.shininess", material.shininess);
}

void Shader::use(const Shader *shader)
{
    if (shader == nullptr) {
        glUseProgram(0);
        return;
    }

    if (shader->m_valid)
        glUseProgram(shader->m_id);
}

uint32_t Shader::getUniformLocation(const std::string &name) const
{
    auto found = m_uniform_locations.find(name);
    if (found == m_uniform_locations.end()) {
        uint32_t location = glGetUniformLocation(m_id, name.c_str());
        m_uniform_locations.try_emplace(name, location);
        return location;
    }
    return found->second;
}

std::pair<uint32_t, bool> Shader::createShaderFromString(const std::string &shader_str,
                                                         int32_t type) const
{
    const GLchar *code = shader_str.c_str();
    GLuint shader;
    GLint success;
    GLchar info_log[512];

    shader = glCreateShader(type);
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        spdlog::error("Shader compilation error: {}", info_log);
        glDeleteShader(shader);
        return {0, false};
    }

    return {shader, true};
}

std::pair<uint32_t, bool> Shader::createProgramm(const std::vector<uint32_t> &shaders) const
{
    GLuint programm = glCreateProgram();

    for (uint32_t shader : shaders)
        glAttachShader(programm, shader);

    glLinkProgram(programm);

    GLint success;
    GLchar info_log[512];

    glGetProgramiv(programm, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programm, 512, nullptr, info_log);
        spdlog::error("Shader compilation error: {}", info_log);
        return {0, false};
    }

    return {programm, true};
}

} // namespace eb
