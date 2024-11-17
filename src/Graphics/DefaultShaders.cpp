#include "DefaultShaders.h"

namespace eb {

const std::string DEFAULT_MAIN_VERTEX_SHADER
    = "#version 330 core\n"

      "layout (location = 0) in vec3 v_position;\n"
      "layout (location = 1) in vec4 v_color;\n"
      "layout (location = 2) in vec2 v_texCoord;\n"

      "uniform mat4 u_model;\n"
      "uniform mat4 u_projection;\n"
      "uniform mat4 u_view;\n"

      "out vec4 a_color;\n"
      "out vec2 a_texCoord;\n"

      "void main(){\n"
      "a_color = v_color;\n"
      "a_texCoord = v_texCoord;\n"

      "gl_Position = u_projection * u_view * (u_model * vec4(v_position, 1.0f));\n"
      "}";

const std::string DEFAULT_MAIN_FRAGMENT_SHADER
    = "#version 330 core\n"

      "in vec4 a_color;\n"
      "in vec2 a_texCoord;\n"
      "out vec4 f_color;\n"

      "uniform sampler2D u_texture0;\n"

      "void main(){\n"
      "    f_color = a_color * texture(u_texture0, a_texCoord);\n"
      "}";

const std::string DEFAULT_MAIN_2D_VERTEX_SHADER
    = "#version 330 core\n"

      "layout (location = 0) in vec3 v_position;\n"
      "layout (location = 1) in vec4 v_color;\n"
      "layout (location = 2) in vec2 v_texCoord;\n"

      "uniform mat4 u_model;\n"
      "uniform mat4 u_projection;\n"

      "out vec4 a_color;\n"
      "out vec2 a_texCoord;\n"

      "void main(){\n"
      "a_color = v_color;\n"
      "a_texCoord = v_texCoord;\n"

      "gl_Position = u_projection * (u_model * vec4(v_position, 1.0f));\n"
      "}";

const std::string DEFAULT_MAIN_2D_FRAGMENT_SHADER
    = "#version 330 core\n"

      "in vec4 a_color;\n"
      "in vec2 a_texCoord;\n"
      "out vec4 f_color;\n"

      "uniform sampler2D u_texture0;\n"

      "void main(){\n"
      "    f_color = a_color * texture(u_texture0, a_texCoord);\n"
      "}";

const std::string DEFAULT_VOXELS_VERTEX_SHADER
    = "#version 330 core\n"

      "layout (location = 0) in vec3 v_position;\n"
      "layout (location = 1) in vec4 v_color;\n"
      "layout (location = 2) in vec2 v_texCoord;\n"
      "layout (location = 3) in float v_light;\n"

      "uniform mat4 u_model;\n"
      "uniform mat4 u_projection;\n"
      "uniform mat4 u_view;\n"

      "out vec4 a_color;\n"
      "out vec2 a_texCoord;\n"

      "void main(){\n"
      "a_color = vec4(v_color.rgb * v_light, v_color.a);\n"
      "a_texCoord = v_texCoord;\n"

      "gl_Position = u_projection * u_view * (u_model * vec4(v_position, 1.0f));\n"
      "}";

const std::string DEFAULT_VOXELS_FRAGMENT_SHADER
    = "#version 330 core\n"

      "in vec4 a_color;\n"
      "in vec2 a_texCoord;\n"
      "out vec4 f_color;\n"

      "uniform sampler2D u_texture0;\n"

      "void main(){\n"
      "    f_color = a_color * texture(u_texture0, a_texCoord);\n"
      "}";

const std::string DEFAULT_LINES_VERTEX_SHADER
    = "#version 330 core\n"

      "layout (location = 0) in vec3 v_position;\n"
      "layout (location = 1) in vec4 v_color;\n"

      "uniform mat4 u_model;\n"
      "uniform mat4 u_projection;\n"
      "uniform mat4 u_view;\n"

      "out vec4 a_color;\n"

      "void main(){\n"
      "a_color = v_color;\n"

      "gl_Position = u_projection * u_view * (u_model * vec4(v_position, 1.0f));\n"
      "}";

const std::string DEFAULT_LINES_FRAGMENT_SHADER = "#version 330 core\n"

                                                  "in vec4 a_color;\n"
                                                  "out vec4 f_color;\n"

                                                  "void main(){\n"
                                                  "    f_color = a_color;\n"
                                                  "}";

std::shared_ptr<Shader> DefaultShaders::getMain()
{
    static auto shader = std::make_shared<Shader>(DEFAULT_MAIN_VERTEX_SHADER,
                                                  DEFAULT_MAIN_FRAGMENT_SHADER);
    return shader;
}

std::shared_ptr<Shader> DefaultShaders::getMain2D()
{
    static auto shader = std::make_shared<Shader>(DEFAULT_MAIN_2D_VERTEX_SHADER,
                                                  DEFAULT_MAIN_2D_FRAGMENT_SHADER);
    return shader;
}

std::shared_ptr<Shader> DefaultShaders::getVoxels()
{
    static auto shader = std::make_shared<Shader>(DEFAULT_VOXELS_VERTEX_SHADER,
                                                  DEFAULT_VOXELS_FRAGMENT_SHADER);
    return shader;
}

std::shared_ptr<Shader> DefaultShaders::getLines()
{
    static auto shader = std::make_shared<Shader>(DEFAULT_LINES_VERTEX_SHADER,
                                                  DEFAULT_LINES_FRAGMENT_SHADER);
    return shader;
}

} // namespace eb
