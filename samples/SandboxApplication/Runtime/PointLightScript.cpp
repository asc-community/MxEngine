#include <MxEngine.h>

using namespace MxEngine;

class PointLightScript : public MxEngine::Scriptable
{
public:
    virtual void OnCreate(MxEngine::MxObject& self) override
    {
        
    }

    virtual void OnReload(MxEngine::MxObject& self) override
    {
        
    }

    virtual void OnUpdate(MxEngine::MxObject& self) override
    {
        self.Transform.Translate(Vector3(1.0f, 0.0f, 0.0f) * Time::Delta());
    }
};

MXENGINE_RUNTIME_EDITOR(PointLightScript);
