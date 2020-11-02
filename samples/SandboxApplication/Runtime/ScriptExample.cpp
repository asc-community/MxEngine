#include <MxEngine.h>
#include <Core/Components/Scripting/Scriptable.h>

using namespace MxEngine;

class ScriptExample : public Scriptable
{
public:
    virtual void OnCreate(MxObject& self) override
    {
        MXLOG_WARNING("ScriptExample", "OnCreate called");
    }

    virtual void OnReload(MxObject& self) override
    {
        MXLOG_WARNING("ScriptExample", "OnReload called");
    }
    
    virtual void OnUpdate(MxObject& self) override
    {
        
    }
};

MXENGINE_RUNTIME_EDITOR(ScriptExample);
