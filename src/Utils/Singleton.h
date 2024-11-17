#ifndef SINGLETON_H
#define SINGLETON_H

#include "NoCopyable.h"

template<typename T>
class Singleton : public NoCopyable
{
public:
    static T &getInstance()
    {
        static T value;
        return value;
    }

protected:
    Singleton() = default;
    ~Singleton() = default;
};

#endif // SINGLETON_H
