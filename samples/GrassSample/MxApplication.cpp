#include <MxEngine.h>

namespace GrassSample
{
    using namespace MxEngine;

    class GrassRenderApplication : public Application
    {
        MxObject::Handle cameraObject;
        MxObject::Handle grass;
        MxObject::Handle lights;
    public:
        void InstanciateGrass()
        {
            auto field = MxObject::Create();
            field->Name = "Grass Field";
            field->Transform.SetScale(20.0f);
            field->AddComponent<MeshSource>(Primitives::CreatePlane());
            auto fieldMaterial = field->AddComponent<MeshRenderer>()->GetMaterial();
            fieldMaterial->AlbedoMap = AssetManager::LoadTexture("Resources/field.png"_id);
            fieldMaterial->UVMultipliers = { 20.0f, 20.0f };

            this->grass = MxObject::Create();
            grass->Name = "Grass Instances";
            grass->Transform.TranslateY(0.3f);
            grass->Transform.ScaleZ(0.75f);

            auto source = grass->AddComponent<MeshSource>(Primitives::CreatePlane2Side());
            source->CastsShadow = false;

            auto material = grass->AddComponent<MeshRenderer>()->GetMaterial();
            material->AlbedoMap = AssetManager::LoadTexture("Resources/grass_al.png"_id, TextureFormat::RGBA);

            auto grassInstances = grass->AddComponent<InstanceFactory>();
            for (size_t i = 0; i < 10000; i++)
            {
                auto g1 = grassInstances->Instanciate();
                auto g2 = grassInstances->Instanciate();
                float x = Random::GetFloat() * 20.0f - 10.0f;
                float z = Random::GetFloat() * 20.0f - 10.0f;
                float r = Random::GetFloat() * 180.0f - 90.0f;
                g1->Transform.TranslateX(x).TranslateZ(z);
                g2->Transform.TranslateX(x).TranslateZ(z);
                g1->Transform.RotateX(-90.0f).RotateY(r);
                g2->Transform.RotateX(-90.0f).RotateY(r - 90.0f);
            }
            grassInstances->SubmitInstances();
            grassInstances->IsStatic = true;
        }

        void AddLighting()
        {
            this->lights = MxObject::Create();
            this->lights->Name = "Light Instances";
            auto source = this->lights->AddComponent<MeshSource>(Primitives::CreateCube());
            source->CastsShadow = false;
            auto material = this->lights->AddComponent<MeshRenderer>()->GetMaterial();
            material->Emission = 200.0f;
            auto lightFactory = this->lights->AddComponent<InstanceFactory>();

            constexpr size_t lightRowSize = 100;

            for (size_t i = 0; i < lightRowSize; i++)
            {
                for (size_t j = 0; j < lightRowSize; j++)
                {
                    auto object = lightFactory->Instanciate();
                    object->Transform.SetPosition(Vector3((float)i - lightRowSize / 2, 0.5f, (float)j - lightRowSize / 2) * 0.2f);
                    object->Transform.Scale(0.1f);

                    auto pointLight = object->AddComponent<PointLight>();
                    pointLight->SetRadius(0.25f);

                    float r = Random::GetFloat();
                    float g = Random::GetFloat();
                    float b = Random::GetFloat();
                    pointLight->SetColor({ r, g, b });
                    pointLight->SetIntensity(100.0f);

                    object->GetComponent<Instance>()->SetColor({ r, g, b });
                }
            }
            lightFactory->SubmitInstances();
            lightFactory->IsStatic = true;
        }

        virtual void OnCreate() override
        {
            Event::AddEventListener<FpsUpdateEvent>("CountFPS",
                [](auto& e)
                {
                    WindowManager::SetTitle(MxFormat("Grass Sample {0} FPS", e.FPS));
                });

            // setup camera
            cameraObject = MxObject::Create();
            cameraObject->Name = "Player Camera";
            cameraObject->Transform.TranslateY(2.0f);
            
            auto skybox = cameraObject->AddComponent<Skybox>();
            skybox->CubeMap = AssetManager::LoadCubeMap("Resources/dawn.jpg"_id);
            skybox->Irradiance = AssetManager::LoadCubeMap("Resources/dawn_irradiance.jpg"_id);
            
            auto effects = cameraObject->AddComponent<CameraEffects>();
            effects->SetBloomIterations(3);

            auto toneMapping = cameraObject->AddComponent<CameraToneMapping>();

            auto controller = cameraObject->AddComponent<CameraController>();
            controller->ListenWindowResizeEvent();

            auto input = cameraObject->AddComponent<InputController>();
            input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
            input->BindRotation();
            
            Rendering::SetViewport(controller);

            // create global directional light
            auto lightObject = MxObject::Create();
            lightObject->Name = "Global Light";
            auto dirLight = lightObject->AddComponent<DirectionalLight>();
            dirLight->Direction = MakeVector3(0.1f, 1.0f, 0.0f);
            dirLight->SetIntensity(35.0f);
            dirLight->FollowViewport();

            this->InstanciateGrass();
            this->AddLighting();
        }

        virtual void OnUpdate() override
        {
            
        }
    };
}

int main()
{
    MxEngine::LaunchFromSourceDirectory();
    GrassSample::GrassRenderApplication app;
    app.Run();
    return 0;
}