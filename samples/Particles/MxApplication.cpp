#include <MxEngine.h>

namespace Particles
{
    using namespace MxEngine;

    class MxApplication : public Application
    {
        virtual void OnCreate() override
        {
            Scene::Load("scene.json"_id);
        }
    };
}

int main()
{
    MxEngine::LaunchFromSourceDirectory();
    Particles::MxApplication app;
    app.Run();
    return 0;
}