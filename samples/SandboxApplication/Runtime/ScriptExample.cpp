#include <MxEngine.h>
#include <Core/Components/Scripting/Scriptable.h>

using namespace MxEngine;

class ScriptExample : public Scriptable
{
public:
    virtual void OnCreate(MxObject& self) override
    {
        
    }

    virtual void OnReload(MxObject& self) override
    {
        
    }
    
    virtual void OnUpdate(MxObject& self) override
    {
        static float maxScale = 5.0f;
        static float scaleSpeed = 1.5f;
        static float rotationSpeed = 15.0f;
        
        if (Runtime::IsEditorActive())
        {
            ImGui::Begin("cube settings");
            ImGui::DragFloat("max scale", &maxScale);
            ImGui::DragFloat("scale speed", &scaleSpeed);
            ImGui::DragFloat("rotation speed", &rotationSpeed);
            ImGui::End();
        }
        
        self.Transform.RotateY(rotationSpeed * Time::Delta());
        auto scale = self.Transform.GetScale().x;
        
        if (scale < maxScale)
        {
            scale = scale + scaleSpeed * Time::Delta();
        }
        else
        {
            scale = scale - scaleSpeed * Time::Delta();
        }
        self.Transform.SetScale(scale);
    }
};

MXENGINE_RUNTIME_EDITOR(ScriptExample);
