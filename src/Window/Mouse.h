#ifndef EB_WINDOW_MOUSE_H
#define EB_WINDOW_MOUSE_H

#include "../EngineObject.h"

#include <stdint.h>
#include <vector>

class GLFWwindow;

namespace eb {

class Window;

class Mouse : public EngineObject
{
    friend class Window;
    friend class Engine;

public:
    bool isButtonPressed(int32_t keycode);
    bool isButtonJustPressed(int32_t keycode);

    int32_t getX();
    int32_t getY();

    int32_t getDeltaX();
    int32_t getDeltaY();

protected:
    Mouse(Engine *engine);
    ~Mouse() = default;

private:
    void updateFrame();
    void updatePositions(double xpos, double ypos);
    void updateState(int32_t button, int32_t action, int32_t mode);

    static void positionCallback(GLFWwindow *glfw_window, double xpos, double ypos);
    static void buttonCallback(GLFWwindow *glfw_window,
                               int32_t button,
                               int32_t action,
                               int32_t mode);

private:
    std::vector<bool> m_buttons;
    std::vector<int32_t> m_frames;
    int32_t m_current_frame;

    float m_delta_x;
    float m_delta_y;
    float m_x;
    float m_y;

    bool m_started;
};

} // namespace eb

#endif // MOUSE_H
