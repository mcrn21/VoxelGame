#include "Mouse.h"
#include "../Engine.h"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#define BUTTONS_COUNT 8

namespace eb {

bool Mouse::isButtonPressed(int32_t keycode)
{
    if (keycode < 0 || keycode >= BUTTONS_COUNT)
        return false;
    return m_buttons[keycode];
}

bool Mouse::isButtonJustPressed(int32_t keycode)
{
    if (keycode < 0 || keycode >= BUTTONS_COUNT)
        return false;

    return m_buttons[keycode] && m_frames[keycode] == m_current_frame;
}

int32_t Mouse::getX()
{
    return m_x;
}

int32_t Mouse::getY()
{
    return m_y;
}

int32_t Mouse::getDeltaX()
{
    return m_delta_x;
}

int32_t Mouse::getDeltaY()
{
    return m_delta_y;
}

Mouse::Mouse(Engine *engine)
    : EngineObject{engine}
    , m_buttons(BUTTONS_COUNT, false)
    , m_frames(BUTTONS_COUNT, 0)
    , m_current_frame{0}
    , m_delta_x{0.0f}
    , m_delta_y{0.0f}
    , m_x{0.0f}
    , m_y{0.0f}
    , m_started{false}
{}

void Mouse::updateFrame()
{
    ++m_current_frame;
    m_delta_x = 0;
    m_delta_y = 0;
}

void Mouse::updatePositions(double xpos, double ypos)
{
    if (m_started) {
        m_delta_x = xpos - m_x;
        m_delta_y = ypos - m_y;

    } else
        m_started = true;

    m_x = xpos;
    m_y = ypos;
}

void Mouse::updateState(int32_t button, int32_t action, int32_t mode)
{
    if (action == GLFW_PRESS) {
        m_buttons[button] = true;
        m_frames[button] = m_current_frame;

    } else if (action == GLFW_RELEASE) {
        m_buttons[button] = false;
        m_frames[button] = m_current_frame;
    }
}

} // namespace eb
