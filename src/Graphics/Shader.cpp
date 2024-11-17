#include "Shader.h"
#include "../Utils/Files.h"

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

Shader::Shader(const std::filesystem::path &vertex_path, const std::filesystem::path &fragment_path)
    : m_id{0}
    , m_valid{false}
{
    loadFromFile(vertex_path, fragment_path);
}

Shader::Shader(const std::string &vertex_string, const std::string &fragment_string)
    : m_id{0}
    , m_valid{false}
{
    loadFromString(vertex_string, fragment_string);
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
                          const std::filesystem::path &fragment_path)
{
    try {
        std::string vertex_string = Files::readFileToString(vertex_path);
        std::string fragment_string = Files::readFileToString(fragment_path);

        return loadFromString(vertex_string, fragment_string);

    } catch (const std::exception &e) {
        spdlog::error("Error loading shader: {}", e.what());
        return false;
    }
}

bool Shader::loadFromString(const std::string &vertex_string, const std::string &fragment_string)
{
    destroy();

    auto vertex_result = createShaderFromString(vertex_string, GL_VERTEX_SHADER);
    if (!vertex_result.second) {
        spdlog::error("Error in shader: {}", vertex_string);
        return false;
    }

    auto fragment_result = createShaderFromString(fragment_string, GL_FRAGMENT_SHADER);
    if (!fragment_result.second) {
        spdlog::error("Error in shader: {}", fragment_string);
        return false;
    }

    auto programm_result = createProgramm({vertex_result.first, fragment_result.first});

    if (programm_result.second) {
        m_valid = true;
        m_id = programm_result.first;
    }

    glDeleteShader(vertex_result.first);
    glDeleteShader(fragment_result.second);

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

void Shader::uniformMatrix(const std::string &name, glm::mat4 matrix)
{
    GLuint transform_loc = glGetUniformLocation(m_id, name.c_str());
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, glm::value_ptr(matrix));
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

std::pair<uint32_t, bool> Shader::createShaderFromString(const std::string &shader_str, int32_t type)
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

std::pair<uint32_t, bool> Shader::createProgramm(const std::vector<uint32_t> &shaders)
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