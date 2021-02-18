#include <MxEngine.h>

namespace SSGI
{
    using namespace MxEngine;

    class SSGIApplication : public Application
    {
        CameraControllerHandle Camera;
        TextureHandle RawSSGI;
        ShaderHandle ShaderSSGI;

        virtual void OnCreate() override
        {
            auto camera = MxObject::Create();
            camera->Name = "Camera";

            this->Camera = camera->AddComponent<CameraController>();
            this->Camera->SetMoveSpeed(10.0f);
            Rendering::SetViewport(this->Camera);

            camera->Transform.TranslateY(15.0f);
            
            auto skybox = camera->AddComponent<Skybox>();
            skybox->CubeMap = AssetManager::LoadCubeMap("Resources/skybox.png"_id);
            skybox->Irradiance = AssetManager::LoadCubeMap("Resources/skybox_irradiance.png"_id);
            skybox->SetIntensity(1.0);
            
            auto input = camera->AddComponent<InputController>();
            input->BindMovementWASDSpaceShift();
            input->BindRotation();

            auto toneMapping = camera->AddComponent<CameraToneMapping>();
            toneMapping->SetMinLuminance(0.3f);
            toneMapping->SetWhitePoint(0.5f);

            auto effects = camera->AddComponent<CameraEffects>();
            effects->SetFogDensity(0.0f);

            auto ssgi = camera->AddComponent<CameraSSGI>();

            auto sponza = MxObject::Create();
            sponza->Name = "Sponza";
            sponza->AddComponent<MeshSource>(AssetManager::LoadMesh("Resources/Sponza/glTF/Sponza.gltf"_id));
            sponza->AddComponent<MeshRenderer>(AssetManager::LoadMaterials("Resources/Sponza/glTF/Sponza.gltf"_id));
            sponza->Transform.SetScale(0.02f);
            sponza->Transform.TranslateY(13.0f);
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