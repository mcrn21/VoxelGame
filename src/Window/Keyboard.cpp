#include "Keyboard.h"
#include "../Engine.h"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#define KEYS_COUNT 1024

namespace eb {

bool Keyboard::isKeyPressed(int32_t keycode)
{
    if (keycode < 0 || keycode >= KEYS_COUNT)
        return false;
    return m_keys[keycode];
}

bool Keyboard::isKeyJustPressed(int32_t keycode)
{
    if (keycode < 0 || keycode >= KEYS_COUNT)
        return false;

    return m_keys[keycode] && m_frames[keycode] == m_current_frame;
}

Keyboard::Keyboard(Engine *engine)
    : EngineObject{engine}
    , m_keys(KEYS_COUNT, false)
    , m_frames(KEYS_COUNT, 0)
    , m_current_frame{0}
{}

void Keyboard::updateFrame()
{
    ++m_current_frame;
}

void Keyboard::updateState(int32_t key, int32_t scancode, int32_t action, int32_t mode)
{
    if (action == GLFW_PRESS && !m_keys[key]) {
        m_keys[key] = true;
        m_frames[key] = m_current_frame;

    } else if (action == GLFW_RELEASE && m_keys[key]) {
        m_keys[key] = false;
        m_frames[key] = m_current_frame;
    }
}

} // namespace eb
