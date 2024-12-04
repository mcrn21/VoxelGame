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
uniform mat4 u_cameraViewMat;

uniform Material u_material;

layout(std140, binding = 0) buffer Lights
{
    WorldLight world_light;
    int length;
    Light lights[];
} u_lights;

uniform sampler2DArray u_shadowMap;
layout (std140, binding = 1) uniform CascadeShadowData
{
    mat4 matrices[16];
    float planes[16];
    int planesCount;
} u_cascade_shadow_data;

out vec4 fragColor;


float calcShadow(vec3 fragPosWorldSpace)
{
    // select cascade layer
    vec4 fragPosViewSpace = u_cameraViewMat * vec4(fragPosWorldSpace, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = -1;
    for (int i = 0; i < u_cascade_shadow_data.planesCount; ++i) {
        if (depthValue < u_cascade_shadow_data.planes[i]) {
            layer = i;
            break;
        }
    }
    if (layer == -1) {
        layer = u_cascade_shadow_data.planesCount;
    }

    vec4 fragPosLightSpace = u_cascade_shadow_data.matrices[layer] * vec4(fragPosWorldSpace, 1.0);
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (currentDepth > 1.0) {
        return 0.0;
    }
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.normal);
    float bias = 0;//max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    const float biasModifier = 0.5f;
    if (layer == u_cascade_shadow_data.planesCount) {
        bias *= 1 / (u_cascade_shadow_data.planes[u_cascade_shadow_data.planesCount] * biasModifier);
    } else {
        bias *= 1 / (u_cascade_shadow_data.planes[layer] * biasModifier);
    }

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(u_shadowMap, 0));
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(u_shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
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


    float shadow = calcShadow(vec3(fs_in.fragPos));

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
