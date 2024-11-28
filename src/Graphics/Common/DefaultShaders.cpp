#include "DefaultShaders.h"

#include <spdlog/spdlog.h>

#define GLSL(str) #str

namespace eb {

const std::string DEFAULT_MAIN_VERTEX_SHADER = R"(
      #version 330 core

      layout (location = 0) in vec3 v_position;
      layout (location = 1) in vec4 v_color;
      layout (location = 2) in vec2 v_texCoord;

      uniform mat4 u_model;
      uniform mat4 u_projection;
      uniform mat4 u_view;

      out vec4 a_color;
      out vec2 a_texCoord;

      void main(){
          a_color = v_color;
          a_texCoord = v_texCoord;
    
          gl_Position = u_projection * u_view * (u_model * vec4(v_position, 1.0f));
      }
)";

const std::string DEFAULT_MAIN_FRAGMENT_SHADER = R"(
      #version 330 core

      in vec4 a_color;
      in vec2 a_texCoord;
      out vec4 f_color;

      uniform sampler2D u_texture0;

      void main(){
          f_color = a_color * texture(u_texture0, a_texCoord);
      }
)";

const std::string DEFAULT_MAIN_2D_VERTEX_SHADER = R"(
      #version 330 core

      layout (location = 0) in vec3 v_position;
      layout (location = 1) in vec4 v_color;
      layout (location = 2) in vec2 v_texCoord;

      uniform mat4 u_model;
      uniform mat4 u_projection;

      out vec4 a_color;
      out vec2 a_texCoord;

      void main(){
          a_color = v_color;
          a_texCoord = v_texCoord;
    
          gl_Position = u_projection * (u_model * vec4(v_position, 1.0f));
      }
)";

const std::string DEFAULT_MAIN_2D_FRAGMENT_SHADER = R"(
      #version 330 core

      in vec4 a_color;
      in vec2 a_texCoord;
      out vec4 f_color;

      uniform sampler2D u_texture0;

      void main(){
          f_color = a_color * texture(u_texture0, a_texCoord);
      }
)";

const std::string DEFAULT_VOXELS_VERTEX_SHADER = R"(
      #version 430 core

      layout (location = 0) in vec3 v_position;
      layout (location = 1) in vec3 v_normal;
      layout (location = 2) in vec2 v_texCoord;
      layout (location = 3) in vec4 v_light;

      uniform mat4 u_model;
      uniform mat4 u_projection;
      uniform mat4 u_view;

      out vec4 a_color;
      out vec2 a_texCoord;
      out vec3 a_normal;
      out vec3 a_fragPos;

      void main(){
          a_color = vec4(v_light.r, v_light.g, v_light.b, 1.0f);
          a_color.rgb += v_light.a;

          a_texCoord = v_texCoord;
          a_normal = mat3(transpose(inverse(u_model))) * v_normal;
          a_fragPos = vec3(u_model * vec4(v_position, 1.0f));
          gl_Position = u_projection * u_view * (u_model * vec4(v_position, 1.0f));
      }
)";

const std::string DEFAULT_VOXELS_FRAGMENT_SHADER = R"(
      #version 430 core

      struct Material {
          sampler2D diffuse_texture0;
          vec4 color;
          vec3 specular;    
          float shininess;
      }; 

      struct WorldLight {
          vec3 direction;
          vec3 ambient;
          vec3 diffuse;
          vec3 specular;
      };

      struct Light
      {
          vec3 position;
          vec3 direction;
          vec3 ambient;
          vec3 diffuse;
          vec3 specular;
          float constant;
          float linear;
          float quadratic;
          float cutOff;
          float outerCutOff;
          bool enable;
      };

      layout(std140, binding = 0) buffer Lights
      {
          WorldLight world_light;
          int length;
          Light lights[];
      } u_lights;

      in vec4 a_color;
      in vec2 a_texCoord;
      in vec3 a_normal;
      in vec3 a_fragPos;

      out vec4 f_color;

      uniform vec3 u_cameraPos;
      uniform Material u_material;

      vec3 calcWorldLight(WorldLight light, Material material, vec3 normal, vec3 cameraDir)
      {
          vec3 lightDir = normalize(-light.direction);

          float diff = max(dot(normal, lightDir), 0.0);

          vec3 reflectDir = reflect(-lightDir, normal);
          float spec = pow(max(dot(cameraDir, reflectDir), 0.0), material.shininess);

          vec3 ambient  = light.ambient  * vec3(texture(material.diffuse_texture0, a_texCoord));
          vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse_texture0, a_texCoord));
          vec3 specular = light.specular * (spec * material.specular);

          return (ambient + diffuse + specular);
      }

      vec3 calcLight(Light light, Material material, vec3 normal, vec3 fragPos, vec3 cameraDir)
      {
          vec3 lightDir = normalize(light.position - fragPos);

          float diff = max(dot(normal, lightDir), 0.0);

          vec3 reflectDir = reflect(-lightDir, normal);
          float spec = pow(max(dot(cameraDir, reflectDir), 0.0), material.shininess);

          float distance = length(light.position - fragPos);
          float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

          vec3 ambient  = light.ambient  * vec3(texture(material.diffuse_texture0, a_texCoord));
          vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse_texture0, a_texCoord));
          vec3 specular = light.specular * spec * (spec * material.specular);

          float intensity = 1.0f;
          if (light.cutOff != 0 && light.outerCutOff != 0) {
            float theta = dot(lightDir, normalize(-light.direction)); 
            float epsilon = light.cutOff - light.outerCutOff;
            intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
          }  
          
          ambient  *= attenuation * intensity;
          diffuse  *= attenuation * intensity;
          specular *= attenuation * intensity;
    
          return (ambient + diffuse + specular);
      }

      void main(){
          vec3 normal = normalize(a_normal);
          vec3 cameraDir = normalize(u_cameraPos - a_fragPos);
          vec3 light = vec3(0.0f);

          light += calcWorldLight(u_lights.world_light, u_material, normal, cameraDir);

          for(int i = 0; i < u_lights.length; ++i) {
               if (u_lights.lights[i].enable == true)
                  light += calcLight(u_lights.lights[i], u_material, normal, a_fragPos, cameraDir);
          }

          f_color = (vec4(light, 1.0f) * a_color);
      }
)";

const std::string DEFAULT_LINES_VERTEX_SHADER = R"(
      #version 330 core

      layout (location = 0) in vec3 v_position;
      layout (location = 1) in vec4 v_color;

      uniform mat4 u_model;
      uniform mat4 u_projection;
      uniform mat4 u_view;

      out vec4 a_color;

      void main(){
          a_color = v_color;
    
          gl_Position = u_projection * u_view * (u_model * vec4(v_position, 1.0f));
      }
)";

const std::string DEFAULT_LINES_FRAGMENT_SHADER = R"(
      #version 330 core

      in vec4 a_color;
      out vec4 f_color;

      void main(){
          f_color = a_color;
      }
)";

const std::string DEFAULT_MESH_VERTEX_SHADER = R"(
      #version 430 core

      layout (location = 0) in vec3 v_position;
      layout (location = 1) in vec3 v_normal;
      layout (location = 2) in vec4 v_color;
      layout (location = 3) in vec2 v_texCoord;

      uniform mat4 u_model;
      uniform mat4 u_projection;
      uniform mat4 u_view;

      out vec4 a_color;
      out vec2 a_texCoord;
      out vec3 a_normal;
      out vec3 a_fragPos;

      void main(){
          a_color = v_color;
          a_texCoord = v_texCoord;
          a_normal = mat3(transpose(inverse(u_model))) * v_normal;
          a_fragPos = vec3(u_model * vec4(v_position, 1.0f));
          gl_Position = u_projection * u_view * (u_model * vec4(v_position, 1.0f));
      }
)";

const std::string DEFAULT_MESH_FRAGMENT_SHADER = R"(
      #version 430 core

      struct Material {
          sampler2D diffuse_texture0;
          vec4 color;
          vec3 specular;    
          float shininess;
      }; 
    
      struct WorldLight {
          vec3 direction;
          vec3 ambient;
          vec3 diffuse;
          vec3 specular;
      };

      struct Light
      {
          vec3 position;
          vec3 direction;
          vec3 ambient;
          vec3 diffuse;
          vec3 specular;
          float constant;
          float linear;
          float quadratic;
          float cutOff;
          float outerCutOff;
          bool enable;
      };

      layout(std140, binding = 0) buffer Lights
      {
          WorldLight world_light;
          int length;
          Light lights[];
      } u_lights;

      in vec4 a_color;
      in vec2 a_texCoord;
      in vec3 a_normal;
      in vec3 a_fragPos;

      out vec4 f_color;

      uniform vec3 u_cameraPos;
      uniform Material u_material;

      vec4 calcWorldLight(WorldLight light, Material material, vec3 normal, vec3 cameraDir)
      {
          vec3 lightDir = normalize(-light.direction);

          float diff = max(dot(normal, lightDir), 0.0);

          vec3 reflectDir = reflect(-lightDir, normal);
          float spec = pow(max(dot(cameraDir, reflectDir), 0.0), material.shininess);

          vec3 ambient  = light.ambient  * vec3(texture(material.diffuse_texture0, a_texCoord));
          vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse_texture0, a_texCoord));
          vec3 specular = light.specular * (spec * material.specular);

          return vec4((ambient + diffuse + specular), texture(material.diffuse_texture0, a_texCoord).a);
      }

      vec4 calcLight(Light light, Material material, vec3 normal, vec3 fragPos, vec3 cameraDir)
      {
          vec3 lightDir = normalize(light.position - fragPos);

          float diff = max(dot(normal, lightDir), 0.0);

          vec3 reflectDir = reflect(-lightDir, normal);
          float spec = pow(max(dot(cameraDir, reflectDir), 0.0), material.shininess);

          float distance = length(light.position - fragPos);
          float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

          vec3 ambient  = light.ambient  * vec3(texture(material.diffuse_texture0, a_texCoord));
          vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse_texture0, a_texCoord));
          vec3 specular = light.specular * spec * (spec * material.specular);

          float intensity = 1.0f;
          if (light.cutOff != 0 && light.outerCutOff != 0) {
            float theta = dot(lightDir, normalize(-light.direction)); 
            float epsilon = light.cutOff - light.outerCutOff;
            intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
          }  
          
          ambient  *= attenuation * intensity;
          diffuse  *= attenuation * intensity;
          specular *= attenuation * intensity;

          return vec4((ambient + diffuse + specular), texture(material.diffuse_texture0, a_texCoord).a);
      }

      void main(){
          if (texture(u_material.diffuse_texture0, a_texCoord).a < 0.005)
            discard;

          vec3 normal = normalize(a_normal);
          vec3 cameraDir = normalize(u_cameraPos - a_fragPos);
          vec4 light = vec4(0.0f);

          light += calcWorldLight(u_lights.world_light, u_material, normal, cameraDir);

          for(int i = 0; i < u_lights.length; ++i) {
               if (u_lights.lights[i].enable == true)
                  light += calcLight(u_lights.lights[i], u_material, normal, a_fragPos, cameraDir);
          }

          f_color = light * a_color;
      }
)";

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

std::shared_ptr<Shader> DefaultShaders::getMesh()
{
    static auto shader = std::make_shared<Shader>(DEFAULT_MESH_VERTEX_SHADER,
                                                  DEFAULT_MESH_FRAGMENT_SHADER);
    return shader;
}

} // namespace eb
