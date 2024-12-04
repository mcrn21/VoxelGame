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

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
    vec4 fragPosLightSpace;
    vec4 color;
} fs_in;

uniform vec3 u_cameraPos;
uniform Material u_material;

layout(std140, binding = 0) buffer Lights
{
    WorldLight world_light;
    int length;
    Light lights[];
} u_lights;

out vec4 fragColor;


vec4 calcWorldLight(WorldLight light, vec3 normal, vec3 cameraDir)
{
    vec3 lightDir = normalize(-light.direction);
    vec3 halfwayDir = normalize(lightDir + cameraDir);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(cameraDir, halfwayDir), 0.0), u_material.shininess);

    vec3 ambient  = light.ambient  * vec3(texture(u_material.diffuse_texture0, fs_in.texCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(u_material.diffuse_texture0, fs_in.texCoords));
    vec3 specular = light.specular * (spec * u_material.specular);


    return vec4((ambient + diffuse + specular), texture(u_material.diffuse_texture0, fs_in.texCoords).a);
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

void main()
{
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
}
