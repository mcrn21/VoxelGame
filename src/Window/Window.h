#ifndef EB_WINDOW_WINDOW_H
#define EB_WINDOW_WINDOW_H

#include "../EngineObject.h"
#include "../Graphics/RenderTarget.h"

#include <string>

class GLFWwindow;

namespace eb {

class Engine;

class Window : public EngineObject, public RenderTarget
{
    friend class Engine;

public:
    bool create(int32_t width, int32_t height, const std::string &title);
    void destroy();

    GLFWwindow *get() const;

    bool isShouldClose() const;
    void setShouldClose(bool flag);

    void swapBuffers();
    void pollEvents();

    bool isMouseEnable() const;
    void setMouseEnable(bool enable);

protected:
    Window(Engine *engine);
    ~Window() = default;

private:
    static void sizeCallback(GLFWwindow *window, int32_t width, int32_t height);
    static void mousePositionCallback(GLFWwindow *glfw_window, double xpos, double ypos);
    static void mouseButtonCallback(GLFWwindow *glfw_window,
                                    int32_t button,
                                    int32_t action,
                                    int32_t mode);
    static void keyCallback(
        GLFWwindow *glfw_window, int32_t key, int32_t scancode, int32_t action, int32_t mode);

private:
    GLFWwindow *m_window;
    bool m_mouse_enable;
};

} // namespace eb

#endif // EB_WINDOW_WINDOW_H
