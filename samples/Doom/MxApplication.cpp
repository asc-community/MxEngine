#include <MxEngine.h>

namespace Doom
{
    using namespace MxEngine;

    /*
    this is MxEngine template project. Just copy it and start developing!
    */
    class MxApplication : public Application
    {
    public:
        virtual void OnCreate() override
        {
            Scene::Load("scene.json");
        }

        virtual void OnUpdate() override
        {
            
        }

        virtual void OnDestroy() override
        {

        }
    };
}

int main()
{
    MxEngine::LaunchFromSourceDirectory();
    Doom::MxApplication app;
    app.Run();
    return 0;
}