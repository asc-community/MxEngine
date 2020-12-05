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
        self.Transform.TranslateX(5.0f * Time::Delta());
    }
};

MXENGINE_RUNTIME_EDITOR(PointLightScript);
