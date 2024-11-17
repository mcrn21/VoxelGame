#include "Engine.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <assert.h>

namespace eb {

Engine *Engine::m_engine = nullptr;

Engine::Engine()
    : m_window{this}
    , m_window_resized{false}
    , m_keyboard{this}
    , m_mouse{this}
    , m_running{false}
    , m_exited{false}
    , m_tick_time{milliseconds(16)}
{
    assert(m_engine == nullptr);
    m_engine = this;
    initGraphics();
}

Engine::~Engine()
{
    terminateGraphics();
}

Engine *Engine::getInstance()
{
    assert(m_engine != nullptr);
    return m_engine;
}

Window &Engine::getWindow()
{
    return m_window;
}

Keyboard &Engine::getKeyboard()
{
    return m_keyboard;
}

Mouse &Engine::getMouse()
{
    return m_mouse;
}

const Time &Engine::getLoopElapsedTime() const
{
    return m_loop_elapsed_time;
}

const Time &Engine::getTickTime() const
{
    return m_tick_time;
}

void Engine::setTickTime(const Time &tick_time)
{
    m_tick_time = tick_time;
}

void Engine::mainLoop()
{
    onInit();

    if (m_exited)
        return;

    m_running = true;

    eb::Time lag;

    while (!m_exited && !m_window.isShouldClose()) {
        lag += (m_loop_elapsed_time = m_loop_clock.restart());

        m_window.pollEvents();

        if (m_window_resized) {
            m_window_resized = false;
            onWindowResize(m_window.getSize().x, m_window.getSize().y);
        }

        onProcessInput(m_loop_elapsed_time);

        while (lag >= m_tick_time) {
            onTick(m_tick_time);
            lag -= m_tick_time;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        onDraw(m_loop_elapsed_time);

        m_window.swapBuffers();
    }

    m_running = false;
    onFinish();
}

void Engine::exit()
{
    m_exited = true;
}

void Engine::onInit() {}

void Engine::onWindowResize(int32_t width, int32_t height) {}

void Engine::onProcessInput(const Time &elapsed) {}

void Engine::onTick(const Time &elapsed) {}

void Engine::onDraw(const Time &elapsed) {}

void Engine::onFinish() {}

void Engine::initGraphics()
{
    glfwInit();
}

void Engine::terminateGraphics()
{
    glfwTerminate();
}

} // namespace eb
