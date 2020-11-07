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
            field->AddComponent<MeshSource>(Primitives::CreatePlane(20));
            auto fieldMaterial = field->AddComponent<MeshRenderer>()->GetMaterial();
            fieldMaterial->AlbedoMap = AssetManager::LoadTexture("field.png"_id);
            fieldMaterial->SpecularFactor = 0.05f;

            this->grass = MxObject::Create();
            grass->Name = "Grass Instances";
            grass->Transform.TranslateY(0.3f);
            grass->Transform.ScaleZ(0.75f);

            grass->AddComponent<MeshSource>(Primitives::CreatePlane());

            auto material = grass->AddComponent<MeshRenderer>()->GetMaterial();
            material->AlbedoMap = AssetManager::LoadTexture("grass_al.png"_id, TextureFormat::RGBA);
            material->SpecularFactor = 0.05f;
            material->CastsShadow = false;

            auto grassInstances = grass->AddComponent<InstanceFactory>();
            for (size_t i = 0; i < 2500; i++)
            {
                auto g1 = grassInstances->MakeInstance();
                auto g2 = grassInstances->MakeInstance();
                float x = Random::GetFloat() * 20.0f - 10.0f;
                float z = Random::GetFloat() * 20.0f - 10.0f;
                float r = Random::GetFloat() * 180.0f - 90.0f;
                g1->Transform.TranslateX(x).TranslateZ(z);
                g2->Transform.TranslateX(x).TranslateZ(z);
                g1->Transform.RotateY(r).RotateX(-90.0f);
                g2->Transform.RotateY(r - 90.0f).RotateX(-90.0f);
            }
            grassInstances->SubmitInstances();
            grassInstances->IsStatic = true;
        }

        void AddLighting()
        {
            this->lights = MxObject::Create();
            this->lights->Name = "Light Instances";
            this->lights->AddComponent<MeshSource>(Primitives::CreateCube());
            auto material = this->lights->AddComponent<MeshRenderer>()->GetMaterial();
            material->CastsShadow = false;
            material->Emmision = 10.0f;
            auto lightFactory = this->lights->AddComponent<InstanceFactory>();

            constexpr size_t lightRowSize = 100;

            for (size_t i = 0; i < lightRowSize; i++)
            {
                for (size_t j = 0; j < lightRowSize; j++)
                {
                    auto object = lightFactory->MakeInstance();
                    object->Transform.SetPosition(Vector3((float)i - lightRowSize / 2, 0.5f, (float)j - lightRowSize / 2) * 0.2f);
                    object->Transform.Scale(0.1f);

                    auto pointLight = object->AddComponent<PointLight>();
                    pointLight->UseRadius(0.4f);

                    float r = Random::GetFloat();
                    float g = Random::GetFloat();
                    float b = Random::GetFloat();
                    pointLight->SetColor({ r, g, b });
                    pointLight->SetIntensity(10.0f);

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
            cameraObject->AddComponent<Skybox>()->Texture = AssetManager::LoadCubeMap("dawn.jpg"_id);
            cameraObject->Transform.TranslateY(2.0f);
            
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
    GrassSample::GrassRenderApplication app;
    app.Run();
    return 0;
}