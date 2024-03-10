#include <MxEngine.h>

namespace Sponza
{
    using namespace MxEngine;

    class SponzaApplication : public Application
    {
        virtual void OnCreate()
        {
            Scene::Load("sponza.json"_id);
        }
    };
}

int main()
{
    MxEngine::LaunchFromSourceDirectory();
    Sponza::SponzaApplication app;
    app.Run();
    return 0;
} 