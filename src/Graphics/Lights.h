#ifndef EB_GRAPHICS_LIGHTS_H
#define EB_GRAPHICS_LIGHTS_H

#include "../Utils/IdStorage.h"
#include "../Utils/SparseSet.h"
#include "GlBuffer.h"

#include <glm/glm.hpp>

#include <memory>

using namespace glm;

namespace eb {

class Light;

class Lights
{
    friend class Light;

public:
    struct WorldLightData
    {
        alignas(16) vec3 direction{-1.0f, -2.5f, -1.0f};
        alignas(16) vec3 ambient{0.5f};
        alignas(16) vec3 diffuse{0.5f};
        alignas(16) vec3 specular{0.5f};
    };

    struct LightData
    {
        alignas(16) vec3 position{0.0f};
        alignas(16) vec3 direction{0.0f};
        alignas(16) vec3 ambient{1.0f};
        alignas(16) vec3 diffuse{1.0f};
        alignas(16) vec3 specular{1.0f};
        alignas(4) float constant = 1.0f;
        alignas(4) float linear = 0.09f;
        alignas(4) float quadratic = 0.032f;
        alignas(4) float cut_off = 0.0f;
        alignas(4) float outer_cut_off = 0.0f;
        alignas(4) bool enable = true;
        uint16_t pad = 0;
    };

    Lights();
    ~Lights() = default;

    const vec3 &getWorldLightDirection() const;
    void setWorldLightDirection(const vec3 &direction);

    const vec3 &getWorldLightAmbient() const;
    void setWorldLightAmbient(const vec3 &ambient);

    const vec3 &getWorldLightDiffuse() const;
    void setWorldLightDiffuse(const vec3 &diffuse);

    const vec3 &getWorldLightSpecular() const;
    void setWorldLightSpecular(const vec3 &specular);

    Light createLight();
    Light getLight(const Id &id);
    void destroyLight(const Id &id);
    bool isLightValid(const Id &id) const;

    const GlBuffer &getLightsSSBO() const;

private:
    WorldLightData m_world_light;
    mutable bool m_world_light_updated;

    IdStorage m_lights_id_storage;
    SparseSet<Id, LightData> m_lights;
    mutable bool m_lights_count_changed;
    mutable std::set<Id> m_lights_updated;
    mutable GlBuffer m_lights_ssbo;
};

class Light
{
public:
    Light();
    Light(const Id &id, Lights *lights);
    ~Light() = default;

    const vec3 &getPosition() const;
    void setPosition(const vec3 &position);

    const vec3 &getDirection() const;
    void setDirection(const vec3 &direction);

    const vec3 &getAmbient() const;
    void setAmbient(const vec3 &ambient);

    const vec3 &getDiffuse() const;
    void setDiffuse(const vec3 &diffuse);

    const vec3 &getSpecular() const;
    void setSpecular(const vec3 &specular);

    float getConstant() const;
    void setConstant(float constant);

    float getLinear() const;
    void setLinear(float linear);

    float getQuadratic() const;
    void setQuadratic(float quadratic);

    float getCutOff() const;
    void setCutOff(float cut_off);

    float getOuterCutOff() const;
    void setOuterCutOff(float outer_cut_off);

    bool isEnable() const;
    void setEnable(bool enable);

    void destroy();
    bool isValid() const;

private:
    Id m_id;
    Lights *m_lights;
};

} // namespace eb

#endif // EB_GRAPHICS_LIGHTS_H
