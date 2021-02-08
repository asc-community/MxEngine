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
            input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
            input->BindRotation();

            auto toneMapping = camera->AddComponent<CameraToneMapping>();
            toneMapping->SetMinLuminance(0.3f);
            toneMapping->SetWhitePoint(0.5f);

            auto effects = camera->AddComponent<CameraEffects>();
            effects->SetFogDensity(0.0f);


            auto sponza = MxObject::Create();
            sponza->Name = "Sponza";
            sponza->AddComponent<MeshSource>(AssetManager::LoadMesh("Resources/Sponza/glTF/Sponza.gltf"_id));
            sponza->AddComponent<MeshRenderer>(AssetManager::LoadMaterials("Resources/Sponza/glTF/Sponza.gltf"_id));
            sponza->Transform.SetScale(0.02f);
            sponza->Transform.TranslateY(13.0f);
            
            // auto dragonFactory = MxObject::Create();
            // dragonFactory->Name = "Dragon Factory";
            // dragonFactory->AddComponent<MeshSource>(AssetManager::LoadMesh("Resources/dragon.obj"_id));
            // auto meshRenderer = dragonFactory->AddComponent<MeshRenderer>(AssetManager::LoadMaterials("Resources/dragon.obj"_id));
            // meshRenderer->GetMaterial()->Emission = 10.0f;
            // 
            // auto factory = dragonFactory->AddComponent<InstanceFactory>();
            // auto dragon1 = factory->Instanciate();
            // auto dragon2 = factory->Instanciate();
            // dragon1->Name = "Blue Dragon";
            // dragon2->Name = "Pink Dragon";
            // 
            // dragon1->Transform
            //     .SetPosition({ -20.0f, 2.0f, 0.0f })
            //     .SetScale(5.0f);
            // dragon2->Transform
            //     .SetPosition({  20.0f, 2.0f, 0.0f })
            //     .SetScale(5.0f);
            // 
            // dragon1->GetComponent<Instance>()->SetColor(Colors::Create(Colors::SKYBLUE));
            // dragon2->GetComponent<Instance>()->SetColor(Colors::Create(Colors::MAGENTA));

            auto lightObject = MxObject::Create();
            lightObject->Transform.SetPosition({ 20.0f, 1.0f, 0.0f });
            auto light = lightObject->AddComponent<PointLight>();
            light->SetColor({ 1.0f, 0.7f, 0.0f });
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