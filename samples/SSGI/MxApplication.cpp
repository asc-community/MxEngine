#include <MxEngine.h>

namespace SSGI
{
    using namespace MxEngine;

    struct Particle
    {
        Vector2 position;
    };

    class SSGIApplication : public Application
    {
        virtual void OnCreate() override
        {
            Scene::Load("scene.json"_id);

            // auto sponza = MxObject::Create();
            // sponza->Name = "Sponza";
            // sponza->AddComponent<MeshSource>(AssetManager::LoadMesh("Resources/Sponza/glTF/Sponza.gltf"_id));
            // sponza->AddComponent<MeshRenderer>(AssetManager::LoadMaterials("Resources/Sponza/glTF/Sponza.gltf"_id));
            // sponza->Transform.SetScale(0.02f);
            // sponza->Transform.TranslateY(13.0f);
        }

        virtual void OnUpdate() override
        {
            
        }
    };
}

int main()
{
    MxEngine::LaunchFromSourceDirectory();
    SSGI::SSGIApplication app;
    app.Run();
    return 0;
}