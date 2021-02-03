#include <MxEngine.h>

using namespace MxEngine;

class ScriptExample : public Scriptable
{
    float totalTime = 0.0f;
public:
    virtual void OnCreate(MxObject& self) override
    {
        
    }

    virtual void OnReload(MxObject& self) override
    {
        totalTime = 0.0f;
        auto position = self.Transform.GetPosition();
        self.Transform.SetPosition({ position.x, 0.5f, position.z });
    }

    virtual void OnUpdate(MxObject& self) override
    {
        Vector3 pos = self.Transform.GetPosition();
        Vector3 scale = self.Transform.GetScale();
        float dt = Time::Delta();
        this->totalTime += dt;
        float offset = 0.003f * std::sin(this->totalTime);

        self.Transform.SetPosition({ pos.x + 3.0f * dt, pos.y + 3.0f * offset, pos.z });
        self.Transform.SetScale({ scale.x, scale.y, scale.z });
    }
};

MXENGINE_RUNTIME_EDITOR(ScriptExample);
