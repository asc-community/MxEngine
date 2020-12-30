#include <MxEngine.h>

namespace PBR
{
    using namespace MxEngine;

    class PBRApplication : public Application
    {
        MeshHandle sphereMesh;

        void GenerateSpheres(const Vector3& startPosition, size_t sphereCount, const Vector3& baseColor)
        {
            for (size_t x = 0; x <= sphereCount; x++)
            {
                for (size_t y = 0; y <= sphereCount; y++)
                {
                    auto sphere = MxObject::Create();
                    sphere->Name = MxFormat("Sphere ({}, {})", x, y);
                    sphere->AddComponent<MeshSource>(sphereMesh);

                    auto material = sphere->AddComponent<MeshRenderer>()->GetMaterial();
                    material->BaseColor = baseColor;
                    material->RoughnessFactor = float(x) / float(sphereCount);
                    material->MetallicFactor = float(y) / float(sphereCount);

                    sphere->Transform.Translate(startPosition);
                    sphere->Transform.Translate(1.5f * MakeVector3((float)x, -1.0f, (float)y));
                }
            }
        }

        virtual void OnCreate() override
        {
            auto cameraObject = MxObject::Create();
            cameraObject->Name = "Camera Object";

            auto controller = cameraObject->AddComponent<CameraController>();
            controller->ListenWindowResizeEvent();
            controller->SetMoveSpeed(10.0f);
            Rendering::SetViewport(controller);

            auto input = cameraObject->AddComponent<InputController>();
            input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
            input->BindRotation();

            auto skybox = cameraObject->AddComponent<Skybox>();
            skybox->CubeMap = AssetManager::LoadCubeMap("Resources/skybox.jpg"_id);
            skybox->Irradiance = AssetManager::LoadCubeMap("Resources/skybox_irradiance.jpg"_id);
            skybox->SetIntensity(1.0f);

            auto toneMapping = cameraObject->AddComponent<CameraToneMapping>();

            sphereMesh = Primitives::CreateSphere();
            
            this->GenerateSpheres(MakeVector3(0.0f,  0.0f, 0.0f ), 5, MakeVector3(1.0f,  0.3f, 0.3f));
            this->GenerateSpheres(MakeVector3(15.0f, 0.0f, 0.0f ), 5, MakeVector3(0.9f,  0.7f, 0.0f));
            this->GenerateSpheres(MakeVector3(0.0f,  0.0f, 15.0f), 5, MakeVector3(1.0f,  1.0f, 1.0f));
            this->GenerateSpheres(MakeVector3(15.0f, 0.0f, 15.0f), 5, MakeVector3(0.4f,  0.4f, 0.4f));
        }
    };
}

int main()
{
    MxEngine::LaunchFromSourceDirectory();
    PBR::PBRApplication app;
    app.Run();
    return 0;
}