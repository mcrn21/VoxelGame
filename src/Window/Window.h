#ifndef EB_WINDOW_WINDOW_H
#define EB_WINDOW_WINDOW_H

#include "../EngineObject.h"
#include "../Graphics/Common/RenderTarget.h"

#include <glm/glm.hpp>

#include <functional>
#include <string>

using namespace glm;

class GLFWwindow;

namespace eb {

class Engine;

class Window : public EngineObject, public RenderTarget
{
    friend class Engine;

public:
    bool create(const i32vec2 &window_size, const std::string &window_title);
    void destroy();

    GLFWwindow *get() const;

    bool isShouldClose() const;
    void setShouldClose(bool flag);

    void pollEvents();

    bool isMouseEnable() const;
    void setMouseEnable(bool enable);

    void setSizeCallback(const std::function<void(const i32vec2 &)> &callback);

    void clear(const vec4 &color = vec4{0.0f}) const;
    void display() const;

protected:
    Window(Engine *engine);
    ~Window();

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
    std::function<void(const i32vec2 &)> m_size_callback;
};

} // namespace eb

#endif // EB_WINDOW_WINDOW_H
