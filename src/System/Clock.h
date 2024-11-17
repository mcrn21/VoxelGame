#ifndef EB_SYSTEM_CLOCK_H
#define EB_SYSTEM_CLOCK_H

#include "Time.h"

namespace eb {

class Clock
{
public:
    Clock();
    ~Clock() = default;

    Time getElapsedTime() const;
    Time restart();

    static Time getCurrentTime();

private:
    Time m_start_time;
};

} // namespace eb

#endif // EB_SYSTEM_CLOCK_H
