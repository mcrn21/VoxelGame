#ifndef EB_ENGINE_H
#define EB_ENGINE_H

#include "Assets/Assets.h"
#include "Scene2D.h"
#include "Scene3D.h"
#include "System/Clock.h"
#include "System/Time.h"
#include "Window/Keyboard.h"
#include "Window/Mouse.h"
#include "Window/Window.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>

using namespace glm;

namespace eb {

class Engine
{
    friend class Window;

public:
    Engine(const i32vec2 &window_size, const std::string &window_title);
    virtual ~Engine();

    static Engine *getInstance();

    Window &getWindow();
    Keyboard &getKeyboard();
    Mouse &getMouse();

    const Time &getLoopElapsedTime() const;

    const Time &getTickTime() const;
    void setTickTime(const Time &tick_time);

    void mainLoop();
    void exit();

    Assets &getAssets();

    std::shared_ptr<Scene3D> getScene3D() const;
    void setScene3D(const std::shared_ptr<Scene3D> &scene);

    std::shared_ptr<Scene2D> getScene2D() const;
    void setScene2D(const std::shared_ptr<Scene2D> &scene);

protected:
    virtual void onInit();
    virtual void onWindowResize(const i32vec2 &window_size);
    virtual void onProcessInput(const Time &elapsed);
    virtual void onTick(const Time &elapsed);
    virtual void onDraw(const Time &elapsed);
    virtual void onFinish();

private:
    void initGraphics();
    void terminateGraphics();

private:
    static Engine *m_engine;

    Window m_window;
    Keyboard m_keyboard;
    Mouse m_mouse;

    bool m_running;
    bool m_exited;
    Clock m_loop_clock;
    Time m_loop_elapsed_time;
    Time m_tick_time;

    Assets m_assets;

    std::shared_ptr<Scene3D> m_scene_3d;
    std::shared_ptr<Scene2D> m_scene_2d;
};

} // namespace eb

#endif // EB_ENGINE_H
