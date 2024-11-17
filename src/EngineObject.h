#ifndef EB_ENGINEOBJECT_H
#define EB_ENGINEOBJECT_H

namespace eb {

class Engine;

class EngineObject
{
public:
    EngineObject(Engine *engine)
        : m_engine{engine}
    {}
    virtual ~EngineObject() = default;

    Engine *getEngine() const { return m_engine; }

private:
    Engine *m_engine;
};

} // namespace eb

#endif // EB_ENGINEOBJECT_H
