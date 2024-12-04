#include "DefaultShaders.h"

#include "battery/embed.hpp"

namespace eb {

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
      layout (location = 3) in vec2 v_texCoords;

      uniform mat4 u_model;
      uniform mat4 u_projViewMat;
      uniform mat4 u_lightSpaceMat;

      out VS_OUT {
          vec3 fragPos;
          vec3 normal;
          vec2 texCoords;
          vec4 fragPosLightSpace;
          vec4 color;
      } vs_out;

      void main(){
          vs_out.color = v_color;
          vs_out.normal = mat3(transpose(inverse(u_model))) * v_normal;
          vs_out.texCoords = v_texCoords;
          vs_out.fragPos = vec3(u_model * vec4(v_position, 1.0f));
          vs_out.fragPosLightSpace = u_lightSpaceMat * vec4(vs_out.fragPos, 1.0);

          gl_Position = u_projViewMat * (u_model * vec4(v_position, 1.0f));
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

      in VS_OUT {
          vec3 fragPos;
          vec3 normal;
          vec2 texCoords;
          vec4 fragPosLightSpace;
          vec4 color;
      } fs_in;

      uniform vec3 u_cameraPos;
      uniform Material u_material;

//      uniform sampler2D u_shadowMap;
//      uniform vec3 u_lightPos;


uniform sampler2DArray shadowMap;

uniform vec3 lightDir;
uniform float farPlane;
uniform mat4 view;

layout (std140, binding = 1) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};

uniform float cascadePlaneDistances[16];
uniform int cascadeCount;

      out vec4 fragColor;



      // float calcShadow(vec4 fragPosLightSpace)
      // {
      //     vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
      //     projCoords = projCoords * 0.5 + 0.5;
      //     float currentDepth = projCoords.z;
      //     vec3 lightDir = normalize(u_lightPos - fs_in.fragPos);
      //     float bias = 0; //max(0.0003 * (1.0 - dot(fs_in.normal, lightDir)), 0.00003);

      //     float shadow = 0.0;
      //     vec2 texelSize = 1.0 / textureSize(u_shadowMap, 0);
      //     int sampleRadius = 1;
      //     for(int x = -sampleRadius; x <= sampleRadius; ++x)
      //     {
      //         for(int y = -sampleRadius; y <= sampleRadius; ++y)
      //         {
      //             float pcfDepth = texture(u_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
      //             if (currentDepth > pcfDepth + bias)
      //               shadow += 1.0f;       
      //         }    
      //     }

      //     shadow /= pow((sampleRadius * 2 + 1), 2);
        
      //     if(projCoords.z > 1.0)
      //         shadow = 0.0;
              
      //     return shadow;
      // }

float calcShadow(vec3 fragPosWorldSpace)
{
    // select cascade layer
    vec4 fragPosViewSpace = view * vec4(fragPosWorldSpace, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = -1;
    for (int i = 0; i < cascadeCount; ++i)
    {
        if (depthValue < cascadePlaneDistances[i])
        {
            layer = i;
            break;
        }
    }
    if (layer == -1)
    {
        layer = cascadeCount;
    }

    vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(fragPosWorldSpace, 1.0);
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (currentDepth > 1.0)
    {
        return 0.0;
    }
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.normal);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    const float biasModifier = 0.5f;
    if (layer == cascadeCount)
    {
        bias *= 1 / (farPlane * biasModifier);
    }
    else
    {
        bias *= 1 / (cascadePlaneDistances[layer] * biasModifier);
    }

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
        
    return shadow;
}

      vec4 calcWorldLight(WorldLight light, vec3 normal, vec3 cameraDir)
      {
          vec3 lightDir = normalize(-light.direction);
          vec3 halfwayDir = normalize(lightDir + cameraDir);

          float diff = max(dot(normal, lightDir), 0.0);
          float spec = pow(max(dot(cameraDir, halfwayDir), 0.0), u_material.shininess);

          vec3 ambient  = light.ambient  * vec3(texture(u_material.diffuse_texture0, fs_in.texCoords));
          vec3 diffuse  = light.diffuse  * diff * vec3(texture(u_material.diffuse_texture0, fs_in.texCoords));
          vec3 specular = light.specular * (spec * u_material.specular);


          float shadow = calcShadow(vec3(fs_in.fragPosLightSpace));  

          return vec4((ambient + (1.0 - shadow) * (diffuse + specular)), texture(u_material.diffuse_texture0, fs_in.texCoords).a);
      }

      vec4 calcLight(Light light, vec3 normal, vec3 fragPos, vec3 cameraDir)
      {
          vec3 lightDir = normalize(light.position - fragPos);
          vec3 halfwayDir = normalize(lightDir + cameraDir);

          float diff = max(dot(normal, lightDir), 0.0);
          float spec = pow(max(dot(cameraDir, halfwayDir), 0.0), u_material.shininess);

          float distance = length(light.position - fragPos);
          float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

          vec3 ambient  = light.ambient  * vec3(texture(u_material.diffuse_texture0, fs_in.texCoords));
          vec3 diffuse  = light.diffuse  * diff * vec3(texture(u_material.diffuse_texture0, fs_in.texCoords));
          vec3 specular = light.specular * spec * (spec * u_material.specular);

          float intensity = 1.0f;
          if (light.cutOff != 0 && light.outerCutOff != 0) {
            float theta = dot(lightDir, normalize(-light.direction)); 
            float epsilon = light.cutOff - light.outerCutOff;
            intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
          }  
          
          ambient  *= attenuation * intensity;
          diffuse  *= attenuation * intensity;
          specular *= attenuation * intensity;

          return vec4((ambient + diffuse + specular), texture(u_material.diffuse_texture0, fs_in.texCoords).a);
      }

      void main(){
          if (texture(u_material.diffuse_texture0, fs_in.texCoords).a < 0.005)
            discard;

          vec3 normal = normalize(fs_in.normal);
          vec3 cameraDir = normalize(u_cameraPos - fs_in.fragPos);
          vec4 light = vec4(0.0f);

          light += calcWorldLight(u_lights.world_light, normal, cameraDir);

          for(int i = 0; i < u_lights.length; ++i) {
               if (u_lights.lights[i].enable == true)
                  light += calcLight(u_lights.lights[i], normal, fs_in.fragPos, cameraDir);
          }

          fragColor = light * fs_in.color;

        // vec3 color = texture(u_material.diffuse_texture0, fs_in.texCoords).rgb;
        // vec3 normal = normalize(fs_in.normal);
        // vec3 lightColor = vec3(0.3);
        // // ambient
        // vec3 ambient = 0.3 * lightColor;
        // // diffuse
        // vec3 lightDir = normalize(u_lightPos - fs_in.fragPos);
        // float diff = max(dot(lightDir, normal), 0.0);
        // vec3 diffuse = diff * lightColor;
        // // specular
        // vec3 viewDir = normalize(u_cameraPos - fs_in.fragPos);
        // vec3 reflectDir = reflect(-lightDir, normal);
        // float spec = 0.0;
        // vec3 halfwayDir = normalize(lightDir + viewDir);  
        // spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
        // vec3 specular = spec * lightColor;    
        // // calculate shadow
        // float shadow = calcShadow(fs_in.fragPosLightSpace);                      
        // vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

        // fragColor = vec4(lighting, 1.0);
      }
)";

const std::string DEFAULT_SHADOWMAP_VERTEX_SHADER = R"(
      #version 430 core

      layout (location = 0) in vec3 v_position;
      layout (location = 1) in vec3 v_normal;
      layout (location = 2) in vec4 v_color;
      layout (location = 3) in vec2 v_texCoord;

      uniform mat4 u_model;
      uniform mat4 u_lightSpaceMat;

      void main(){
          gl_Position = u_lightSpaceMat * (u_model * vec4(v_position, 1.0f));
      }
)";

const std::string DEFAULT_SHADOWMAP_FRAGMENT_SHADER = R"(
      #version 430 core

      void main(){
      }
)";

const std::string DEFAULT_CASCADE_SHADOWMAP_VERTEX_SHADER = R"(
      #version 430 core

      layout (location = 0) in vec3 v_position;

      uniform mat4 u_model;

      void main(){
          gl_Position = u_model * vec4(v_position, 1.0f);
      }
)";

const std::string DEFAULT_CASCADE_SHADOWMAP_GEOMETRY_SHADER = R"(
    #version 430 core
        
    layout(triangles, invocations = 5) in;
    layout(triangle_strip, max_vertices = 3) out;
        
    layout (std140, binding = 1) uniform LightSpaceMatrices
    {
        mat4 lightSpaceMatrices[16];
    };
        
    void main()
    {          
        for (int i = 0; i < 3; ++i)
        {
            gl_Position = 
                lightSpaceMatrices[gl_InvocationID] * gl_in[i].gl_Position;
            gl_Layer = gl_InvocationID;
            EmitVertex();
        }
        EndPrimitive();
    }  
)";

std::shared_ptr<Shader> DefaultShaders::getMain()
{
    static auto shader = std::make_shared<Shader>(b::embed<"shaders/main.vert">().str(),
                                                  std::string{},
                                                  b::embed<"shaders/main.frag">().str());
    return shader;
}

std::shared_ptr<Shader> DefaultShaders::getMainShadowMap()
{
    static auto shader = std::make_shared<Shader>(b::embed<"shaders/main.vert">().str(),
                                                  std::string{},
                                                  b::embed<"shaders/main_shadow_map.frag">().str());
    return shader;
}

std::shared_ptr<Shader> DefaultShaders::getMainCascadeShadowMap()
{
    static auto shader
        = std::make_shared<Shader>(b::embed<"shaders/main.vert">().str(),
                                   std::string{},
                                   b::embed<"shaders/main_cascade_shadow_map.frag">().str());
    return shader;
}

std::shared_ptr<Shader> DefaultShaders::getMain2D()
{
    static auto shader = std::make_shared<Shader>(b::embed<"shaders/main_2d.vert">().str(),
                                                  std::string{},
                                                  b::embed<"shaders/main_2d.frag">().str());
    return shader;
}

std::shared_ptr<Shader> DefaultShaders::getShadowMap()
{
    static auto shader = std::make_shared<Shader>(b::embed<"shaders/shadow_map.vert">().str(),
                                                  std::string{},
                                                  std::string{});
    return shader;
}

std::shared_ptr<Shader> DefaultShaders::getCasacadeShadowMap()
{
    static auto shader
        = std::make_shared<Shader>(b::embed<"shaders/cascade_shadow_map.vert">().str(),
                                   b::embed<"shaders/cascade_shadow_map.geom">().str(),
                                   std::string{});
    return shader;
}

std::shared_ptr<Shader> DefaultShaders::getVoxels()
{
    static auto shader = std::make_shared<Shader>(DEFAULT_VOXELS_VERTEX_SHADER,
                                                  std::string{},
                                                  DEFAULT_VOXELS_FRAGMENT_SHADER);
    return shader;
}

std::shared_ptr<Shader> DefaultShaders::getLines()
{
    static auto shader = std::make_shared<Shader>(DEFAULT_LINES_VERTEX_SHADER,
                                                  std::string{},
                                                  DEFAULT_LINES_FRAGMENT_SHADER);
    return shader;
}

std::shared_ptr<Shader> DefaultShaders::getMesh()
{
    static auto shader = std::make_shared<Shader>(DEFAULT_MESH_VERTEX_SHADER,
                                                  std::string{},
                                                  DEFAULT_MESH_FRAGMENT_SHADER);
    return shader;
}

std::shared_ptr<Shader> DefaultShaders::getShadowmap()
{
    static auto shader = std::make_shared<Shader>(DEFAULT_SHADOWMAP_VERTEX_SHADER,
                                                  std::string{},
                                                  DEFAULT_SHADOWMAP_FRAGMENT_SHADER);
    return shader;
}

std::shared_ptr<Shader> DefaultShaders::getCascadeShadowmap()
{
    static auto shader = std::make_shared<Shader>(DEFAULT_CASCADE_SHADOWMAP_VERTEX_SHADER,
                                                  DEFAULT_CASCADE_SHADOWMAP_GEOMETRY_SHADER,
                                                  std::string{});
    return shader;
}

} // namespace eb
