#include <MxEngine.h>

#include <Core/Components/Scripting/Scriptable.h>

MXENGINE_RUNTIME_LINKLIBRARY("MxEngine.lib");
MXENGINE_RUNTIME_LINKLIBRARY("fmtd.lib");
MXENGINE_RUNTIME_LINKLIBRARY("libglew_static.lib");
MXENGINE_RUNTIME_LINKLIBRARY("assimp-vc142-mt.lib");
MXENGINE_RUNTIME_LINKLIBRARY("IrrXML.lib");
MXENGINE_RUNTIME_LINKLIBRARY("zlibstaticd.lib");
MXENGINE_RUNTIME_LINKLIBRARY("glfw3dll.lib");
MXENGINE_RUNTIME_LINKLIBRARY("imgui.lib");
MXENGINE_RUNTIME_LINKLIBRARY("EASTL.lib");
MXENGINE_RUNTIME_LINKLIBRARY("BulletDynamics.lib");
MXENGINE_RUNTIME_LINKLIBRARY("BulletCollision.lib");
MXENGINE_RUNTIME_LINKLIBRARY("LinearMath.lib");
MXENGINE_RUNTIME_LINKLIBRARY("Opengl32.lib");
MXENGINE_RUNTIME_LINKLIBRARY("OpenAL32.lib");
MXENGINE_RUNTIME_LINKLIBRARY("RuntimeObjectSystem.lib");
MXENGINE_RUNTIME_LINKLIBRARY("RuntimeCompiler.lib");
MXENGINE_RUNTIME_LINKLIBRARY("Shell32.lib");
MXENGINE_RUNTIME_LINKLIBRARY("Gdi32.lib");
MXENGINE_RUNTIME_LINKLIBRARY("Advapi32.lib");

class ScriptExample : public MxEngine::Scriptable
{
public:
    virtual void OnUpdate() override
    {
        auto cube = MxEngine::MxObject::GetByName("Sphere");
        cube->Transform.Translate(MxEngine::Vector3(1.0f, 0.0f, 1.0f) * MxEngine::Time::Delta());
        
    }
};

MXENGINE_RUNTIME_EDITOR(ScriptExample);
