#include "Lights.h"

#include <spdlog/spdlog.h>

#include <stdexcept>

#define PADDING_16B 16

namespace eb {

Lights::Lights()
    : m_world_light_updated{false}
    , m_lights_ssbo{SHADER_STORAGE_BUFFER}
    , m_lights_count_changed{true}
{}

const vec3 &Lights::getWorldLightDirection() const
{
    return m_world_light.direction;
}

void Lights::setWorldLightDirection(const vec3 &direction)
{
    m_world_light.direction = direction;
    m_world_light_updated = true;
}

const vec3 &Lights::getWorldLightAmbient() const
{
    return m_world_light.ambient;
}

void Lights::setWorldLightAmbient(const vec3 &ambient)
{
    m_world_light.ambient = ambient;
    m_world_light_updated = true;
}

const vec3 &Lights::getWorldLightDiffuse() const
{
    return m_world_light.diffuse;
}

void Lights::setWorldLightDiffuse(const vec3 &diffuse)
{
    m_world_light.diffuse = diffuse;
    m_world_light_updated = true;
}

const vec3 &Lights::getWorldLightSpecular() const
{
    return m_world_light.specular;
}

void Lights::setWorldLightSpecular(const vec3 &specular)
{
    m_world_light.specular = specular;
    m_world_light_updated = true;
}

Light Lights::createLight()
{
    Id id = m_lights_id_storage.create();
    m_lights.insert(id);
    m_lights_count_changed = true;
    return Light{id, this};
}

Light Lights::getLight(const Id &id)
{
    if (!m_lights_id_storage.isValid(id))
        return Light{Id::invalid, nullptr};

    return Light{id, this};
}

void Lights::destroyLight(const Id &id)
{
    if (!m_lights_id_storage.isValid(id))
        return;

    m_lights.remove(id);
    m_lights_id_storage.destroy(id);
    m_lights_count_changed = true;
}

bool Lights::isLightValid(const Id &id) const
{
    return m_lights_id_storage.isValid(id);
}

const GlBuffer &Lights::getLightsSSBO() const
{
    if (m_lights_count_changed) {
        int32_t lights_size = m_lights.getSize();
        int32_t full_size = sizeof(WorldLightData) + PADDING_16B + lights_size * sizeof(LightData);

        m_lights_ssbo.create(full_size);

        m_lights_ssbo.setData(&m_world_light, sizeof(WorldLightData));
        m_lights_ssbo.setData(&lights_size, sizeof(int32_t), sizeof(WorldLightData));
        m_lights_ssbo.setData(m_lights.getData().data(),
                              lights_size * sizeof(LightData),
                              sizeof(WorldLightData) + PADDING_16B);

        m_lights_count_changed = false;
        m_world_light_updated = false;
        m_lights_updated.clear();

    } else {
        if (m_world_light_updated) {
            m_world_light_updated = false;
            m_lights_ssbo.setData(&m_world_light);
        }

        if (!m_lights_updated.empty()) {
            const auto &light_datas = m_lights.getData();
            for (const auto &id : m_lights_updated) {
                int32_t data_index = m_lights.getIndex(id);
                m_lights_ssbo.setData(&light_datas[data_index],
                                      sizeof(LightData),
                                      sizeof(WorldLightData) + PADDING_16B
                                          + data_index * sizeof(LightData));
            }

            m_lights_updated.clear();
        }
    }

    return m_lights_ssbo;
}

Light::Light()
    : m_id{Id::invalid}
    , m_lights{nullptr}
{}

Light::Light(const Id &id, Lights *lights)
    : m_id{id}
    , m_lights{lights}
{}

const vec3 &Light::getPosition() const
{
    if (!isValid())
        throw std::runtime_error{"Invalid spot light"};
    return m_lights->m_lights.get(m_id).position;
}

void Light::setPosition(const vec3 &position)
{
    if (!isValid())
        return;
    m_lights->m_lights.get(m_id).position = position;
    m_lights->m_lights_updated.insert(m_id);
}

const vec3 &Light::getDirection() const
{
    if (!isValid())
        throw std::runtime_error{"Invalid spot light"};
    return m_lights->m_lights.get(m_id).direction;
}

void Light::setDirection(const vec3 &direction)
{
    if (!isValid())
        return;
    m_lights->m_lights.get(m_id).direction = direction;
    m_lights->m_lights_updated.insert(m_id);
}

const vec3 &Light::getAmbient() const
{
    if (!isValid())
        throw std::runtime_error{"Invalid spot light"};
    return m_lights->m_lights.get(m_id).ambient;
}

void Light::setAmbient(const vec3 &ambient)
{
    if (!isValid())
        return;
    m_lights->m_lights.get(m_id).ambient = ambient;
    m_lights->m_lights_updated.insert(m_id);
}

const vec3 &Light::getDiffuse() const
{
    if (!isValid())
        throw std::runtime_error{"Invalid spot light"};
    return m_lights->m_lights.get(m_id).diffuse;
}

void Light::setDiffuse(const vec3 &diffuse)
{
    if (!isValid())
        return;
    m_lights->m_lights.get(m_id).diffuse = diffuse;
    m_lights->m_lights_updated.insert(m_id);
}

const vec3 &Light::getSpecular() const
{
    if (!isValid())
        throw std::runtime_error{"Invalid spot light"};
    return m_lights->m_lights.get(m_id).specular;
}

void Light::setSpecular(const vec3 &specular)
{
    if (!isValid())
        return;
    m_lights->m_lights.get(m_id).specular = specular;
    m_lights->m_lights_updated.insert(m_id);
}

float Light::getConstant() const
{
    if (!isValid())
        throw std::runtime_error{"Invalid spot light"};
    return m_lights->m_lights.get(m_id).constant;
}

void Light::setConstant(float constant)
{
    if (!isValid())
        return;
    m_lights->m_lights.get(m_id).constant = constant;
    m_lights->m_lights_updated.insert(m_id);
}

float Light::getLinear() const
{
    if (!isValid())
        throw std::runtime_error{"Invalid spot light"};
    return m_lights->m_lights.get(m_id).linear;
}

void Light::setLinear(float linear)
{
    if (!isValid())
        return;
    m_lights->m_lights.get(m_id).linear = linear;
    m_lights->m_lights_updated.insert(m_id);
}

float Light::getQuadratic() const
{
    if (!isValid())
        throw std::runtime_error{"Invalid spot light"};
    return m_lights->m_lights.get(m_id).quadratic;
}

void Light::setQuadratic(float quadratic)
{
    if (!isValid())
        return;
    m_lights->m_lights.get(m_id).quadratic = quadratic;
    m_lights->m_lights_updated.insert(m_id);
}

float Light::getCutOff() const
{
    if (!isValid())
        throw std::runtime_error{"Invalid spot light"};
    return m_lights->m_lights.get(m_id).cut_off;
}

void Light::setCutOff(float cut_off)
{
    if (!isValid())
        return;
    m_lights->m_lights.get(m_id).cut_off = cut_off;
    m_lights->m_lights_updated.insert(m_id);
}

float Light::getOuterCutOff() const
{
    if (!isValid())
        throw std::runtime_error{"Invalid spot light"};
    return m_lights->m_lights.get(m_id).outer_cut_off;
}

void Light::setOuterCutOff(float outer_cut_off)
{
    if (!isValid())
        return;
    m_lights->m_lights.get(m_id).outer_cut_off = outer_cut_off;
    m_lights->m_lights_updated.insert(m_id);
}

bool Light::isEnable() const
{
    if (!isValid())
        throw std::runtime_error{"Invalid spot light"};
    return m_lights->m_lights.get(m_id).enable;
}

void Light::setEnable(bool enable)
{
    if (!isValid())
        return;
    m_lights->m_lights.get(m_id).enable = enable;
    m_lights->m_lights_updated.insert(m_id);
}

void Light::destroy()
{
    if (!isValid())
        return;

    m_lights->destroyLight(m_id);
    m_lights = nullptr;
    m_id = Id::invalid;
}

bool Light::isValid() const
{
    return m_lights && m_lights->isLightValid(m_id);
}

} // namespace eb
