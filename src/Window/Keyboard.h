#ifndef EB_WINDOW_KEYBOARD_H
#define EB_WINDOW_KEYBOARD_H

#include "../EngineObject.h"

#include <stdint.h>
#include <vector>

class GLFWwindow;

namespace eb {

class Window;

class Keyboard : public EngineObject
{
    friend class Window;
    friend class Engine;

public:
    bool isKeyPressed(int32_t keycode);
    bool isKeyJustPressed(int32_t keycode);

protected:
    Keyboard(Engine *engine);
    ~Keyboard() = default;

private:
    void updateFrame();
    void updateState(int32_t key, int32_t scancode, int32_t action, int32_t mode);

private:
    std::vector<bool> m_keys;
    std::vector<int32_t> m_frames;
    int32_t m_current_frame;
};

} // namespace eb

#endif // EB_WINDOW_KEYBOARD_H
