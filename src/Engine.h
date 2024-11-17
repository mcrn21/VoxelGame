#ifndef EB_ENGINE_H
#define EB_ENGINE_H

#include "System/Clock.h"
#include "System/Time.h"
#include "Window/Keyboard.h"
#include "Window/Mouse.h"
#include "Window/Window.h"

namespace eb {

class Engine
{
    friend class Window;

public:
    Engine();
    virtual ~Engine();

    static Engine *getInstance();

    Window &getWindow();
    Keyboard &getKeyboard();
    Mouse &getMouse();

    Camera &getCamera();

    const Time &getLoopElapsedTime() const;

    const Time &getTickTime() const;
    void setTickTime(const Time &tick_time);

    void mainLoop();
    void exit();

protected:
    virtual void onInit();
    virtual void onWindowResize(int32_t width, int32_t height);
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
    bool m_window_resized;

    Keyboard m_keyboard;
    Mouse m_mouse;

    bool m_running;
    bool m_exited;
    Clock m_loop_clock;
    Time m_loop_elapsed_time;
    Time m_tick_time;
};

} // namespace eb

#endif // EB_ENGINE_H
