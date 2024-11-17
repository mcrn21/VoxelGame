#include "Window.h"
#include "../Engine.h"
#include "../Graphics/Drawable.h"
#include "Keyboard.h"
#include "Mouse.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <spdlog/spdlog.h>

namespace eb {

bool Window::create(int32_t width, int32_t height, const std::string &title)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if (m_window == nullptr) {
        spdlog::error("Failed to create GLFW Window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(m_window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        spdlog::error("Failed to initialize GLEW");
        return false;
    }

    glfwSetMouseButtonCallback(m_window, &Window::mouseButtonCallback);
    glfwSetCursorPosCallback(m_window, &Window::mousePositionCallback);
    glfwSetWindowSizeCallback(m_window, &Window::sizeCallback);
    glfwSetKeyCallback(m_window, &Window::keyCallback);

    glfwSetWindowUserPointer(m_window, this);

    setSize({width, height});
    getEngine()->m_window_resized = true;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void Window::destroy()
{
    glfwDestroyWindow(m_window);
    m_window = nullptr;
}

GLFWwindow *Window::get() const
{
    return m_window;
}

bool Window::isShouldClose() const
{
    assert(m_window != nullptr);
    return glfwWindowShouldClose(m_window);
}

void Window::setShouldClose(bool flag)
{
    assert(m_window != nullptr);
    glfwSetWindowShouldClose(m_window, flag);
}

void Window::swapBuffers()
{
    assert(m_window != nullptr);
    glfwSwapBuffers(m_window);
}

void Window::pollEvents()
{
    getEngine()->getKeyboard().updateFrame();
    getEngine()->getMouse().updateFrame();
    glfwPollEvents();
}

bool Window::isMouseEnable() const
{
    return m_mouse_enable;
}

void Window::setMouseEnable(bool enable)
{
    assert(m_window != nullptr);
    m_mouse_enable = !m_mouse_enable;
    glfwSetInputMode(m_window,
                     GLFW_CURSOR,
                     m_mouse_enable ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

Window::Window(Engine *engine)
    : EngineObject{engine}
    , RenderTarget()
    , m_window{nullptr}
    , m_mouse_enable{true}
{}

void Window::sizeCallback(GLFWwindow *window, int32_t width, int32_t height)
{
    Window *w = static_cast<Window *>(glfwGetWindowUserPointer(window));
    w->setSize({width, height});
    w->getEngine()->m_window_resized = true;
}

void Window::mousePositionCallback(GLFWwindow *glfw_window, double xpos, double ypos)
{
    Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
    Mouse &mouse = window->getEngine()->getMouse();
    mouse.updatePositions(xpos, ypos);
}

void Window::mouseButtonCallback(GLFWwindow *glfw_window,
                                 int32_t button,
                                 int32_t action,
                                 int32_t mode)
{
    Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
    Mouse &mouse = window->getEngine()->getMouse();
    mouse.updateState(button, action, mode);
}

void Window::keyCallback(
    GLFWwindow *glfw_window, int32_t key, int32_t scancode, int32_t action, int32_t mode)
{
    Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
    Keyboard &keyboard = window->getEngine()->getKeyboard();
    keyboard.updateState(key, scancode, action, mode);
}

} // namespace eb
