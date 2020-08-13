#include <MxEngine.h>

namespace GrassSample
{
    using namespace MxEngine;

    class GrassRenderApplication : public Application
    {
        MxObject::Handle cameraObject;
        MxObject::Handle grass;
    public:
        virtual void OnCreate() override
        {
            // setup camera
            cameraObject = MxObject::Create();
            cameraObject->Name = "Player Camera";
            cameraObject->AddComponent<Skybox>()->Texture = AssetManager::LoadCubeMap("dawn.jpg"_id);
            cameraObject->Transform.TranslateY(2.0f);
            auto controller = cameraObject->AddComponent<CameraController>();
            auto input = cameraObject->AddComponent<InputControl>();
            controller->ListenWindowResizeEvent();
            input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
            input->BindRotation();
            Rendering::SetViewport(controller);

            // create global directional light
            auto lightObject = MxObject::Create();
            lightObject->Name = "Global Light";
            auto dirLight = lightObject->AddComponent<DirectionalLight>();
            dirLight->Direction = MakeVector3(0.1f, 1.0f, 0.0f);
            dirLight->FollowViewport();

            auto field = MxObject::Create();
            field->AddComponent<MeshSource>(Primitives::CreatePlane(11));
            auto fieldMaterial = field->AddComponent<MeshRenderer>()->GetMaterial();
            fieldMaterial->AlbedoMap = AssetManager::LoadTexture("field.png"_id);
            fieldMaterial->SpecularFactor = 0.05f;
            
            this->grass = MxObject::Create();
            grass->Transform.TranslateY(0.3f);
            grass->Transform.ScaleZ(0.75f);

            grass->AddComponent<MeshSource>(Primitives::CreatePlane());

            auto material = grass->AddComponent<MeshRenderer>()->GetMaterial();
            material->AlbedoMap = AssetManager::LoadTexture("grass_al.png"_id);
            material->TransparencyMap = AssetManager::LoadTexture("grass_tr.png"_id);
            material->SpecularFactor = 0.05f;
            material->CastsShadow = false;

            auto grassInstances = grass->AddComponent<InstanceFactory>();
            for (size_t i = 0; i < 1000; i++)
            {
                auto g1 = grassInstances->MakeInstance();
                auto g2 = grassInstances->MakeInstance();
                float x = Random::GetFloat() * 10.0f - 5.0f;
                float z = Random::GetFloat() * 10.0f - 5.0f;
                float r = Random::GetFloat() * 180.0f - 90.0f;
                g1->Transform.TranslateX(x).TranslateZ(z);
                g2->Transform.TranslateX(x).TranslateZ(z);
                g1->Transform.RotateY(r).RotateX(-90.0f);
                g2->Transform.RotateY(r - 90.0f).RotateX(-90.0f);
            }
            grassInstances->SubmitInstances();
            grassInstances->IsStatic = true;
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