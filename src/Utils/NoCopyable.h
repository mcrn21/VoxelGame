#ifndef NOCOPYABLE_H
#define NOCOPYABLE_H

class NoCopyable
{
protected:
    NoCopyable() = default;
    virtual ~NoCopyable() = default;

private:
    NoCopyable(const NoCopyable &) = delete;
    NoCopyable &operator=(const NoCopyable &) = delete;
};

#endif // NOCOPYABLE_H
