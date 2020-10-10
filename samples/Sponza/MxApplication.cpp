#include <MxEngine.h>

namespace Sponza
{
    using namespace MxEngine;

    class SponzaApplication : public Application
    {
        virtual void OnCreate() override
        {
            auto camera = MxObject::Create();
            camera->Name = "Camera";

            auto controller = camera->AddComponent<CameraController>();
            controller->SetMoveSpeed(10.0f);

            auto input = camera->AddComponent<InputController>();
            input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
            input->BindRotation();

            auto toneMapping = camera->AddComponent<CameraToneMapping>();
            toneMapping->SetEyeAdaptation(0.05f);
            toneMapping->SetMinLuminance(0.3f);
            toneMapping->SetWhitePoint(0.75f);

            auto ssr = camera->AddComponent<CameraSSR>();
            ssr->SetSteps(10);

            Rendering::SetViewport(controller);
            Rendering::SetFogDensity(0.0f);

            auto lightObject = MxObject::Create();
            lightObject->Name = "Global Light";
            auto dirLight = lightObject->AddComponent<DirectionalLight>();
            dirLight->ProjectionSize = 150.0f;
            dirLight->FollowViewport();

            auto sponza = MxObject::Create();
            sponza->Name = "Sponza";
            sponza->AddComponent<MeshSource>(AssetManager::LoadMesh("Sponza/glTF/Sponza.gltf"_id));
            sponza->AddComponent<MeshRenderer>(AssetManager::LoadMaterials("Sponza/glTF/Sponza.gltf"_id));
            sponza->Transform.SetScale(0.02f);
            sponza->Transform.TranslateY(13.0f);
            sponza->GetComponent<MeshRenderer>()->Materials[8]->Reflection = 0.75f;
        }

        virtual void OnUpdate() override
        {
           
        }
    };
}

int main()
{
    Sponza::SponzaApplication app;
    app.Run();
    return 0;
}