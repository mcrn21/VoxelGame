#include "Engine.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <assert.h>

namespace eb {

Engine *Engine::m_engine = nullptr;

Engine::Engine(const i32vec2 &window_size, const std::string &window_title)
    : m_window{this}
    , m_keyboard{this}
    , m_mouse{this}
    , m_running{false}
    , m_exited{false}
    , m_tick_time{milliseconds(16)}
{
    assert(m_engine == nullptr);
    m_engine = this;
    initGraphics();

    m_window.setSizeCallback(std::bind(&Engine::onWindowResize, this, std::placeholders::_1));

    m_window.create(window_size, window_title);

    if (m_window.get()) {
        m_scene_3d = std::make_shared<Scene3D>();
        m_scene_2d = std::make_shared<Scene2D>();
    }
}

Engine::~Engine()
{
    m_window.destroy();
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
    if (!m_window.get()) {
        spdlog::warn("No window");
        return;
    }

    onInit();
    onWindowResize(m_window.getSize());

    if (m_exited)
        return;

    m_running = true;

    eb::Time lag;

    while (!m_exited && !m_window.isShouldClose()) {
        lag += (m_loop_elapsed_time = m_loop_clock.restart());

        m_window.pollEvents();
        onProcessInput(m_loop_elapsed_time);

        while (lag >= m_tick_time) {
            onTick(m_tick_time);
            lag -= m_tick_time;
        }

        m_window.clear();
        onDraw(m_loop_elapsed_time);
        m_window.display();
    }

    m_running = false;
    onFinish();
}

void Engine::exit()
{
    m_exited = true;
}

Assets &Engine::getAssets()
{
    return m_assets;
}

std::shared_ptr<Scene3D> Engine::getScene3D() const
{
    return m_scene_3d;
}

void Engine::setScene3D(const std::shared_ptr<Scene3D> &scene)
{
    if (!scene)
        return;
    m_scene_3d = scene;
}

std::shared_ptr<Scene2D> Engine::getScene2D() const
{
    return m_scene_2d;
}

void Engine::setScene2D(const std::shared_ptr<Scene2D> &scene)
{
    m_scene_2d = scene;
}

void Engine::onInit() {}

void Engine::onWindowResize(const i32vec2 &window_size)
{
    m_window.setViewport({0, 0, window_size.x, window_size.y});
    m_scene_3d->updateProjectionMatrix(window_size);
    m_scene_2d->updateProjectionMatrix(window_size);
}

void Engine::onProcessInput(const Time &elapsed) {}

void Engine::onTick(const Time &elapsed)
{
    m_scene_3d->update(elapsed);
}

void Engine::onDraw(const Time &elapsed)
{
    m_scene_3d->draw(elapsed, m_window);
}

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
