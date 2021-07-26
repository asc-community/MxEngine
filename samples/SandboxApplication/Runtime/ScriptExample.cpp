#include <MxEngine.h>

using namespace MxEngine;

class ScriptExample : public Scriptable
{
    inline static float totalTime = 0.0f;
public:
    virtual void OnCreate(MxObject::Handle self) override
    {
        
    }

    virtual void OnReload(MxObject::Handle self) override
    {
        totalTime = 0.0f;
        auto position = self->LocalTransform.GetPosition();
        self->LocalTransform.SetPosition({ position.x, 0.5f, position.z });
    }

    virtual void OnUpdate(MxObject::Handle self) override
    {
        Vector3 pos = self->LocalTransform.GetPosition();
        Vector3 scale = self->LocalTransform.GetScale();
        float dt = Time::Delta();
        totalTime += dt;
        float offset = 0.003f * std::sin(totalTime);

        self->LocalTransform.SetPosition({ pos.x + 3.0f * dt, pos.y + 3.0f * offset, pos.z });
        self->LocalTransform.SetScale({ scale.x, scale.y, scale.z });
    }
};

MXENGINE_RUNTIME_EDITOR(ScriptExample);
