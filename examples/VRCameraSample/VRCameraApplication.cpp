#pragma once

#include <MxEngine.h>
#include <Library/Primitives/Colors.h>

namespace ProjectTemplate
{
    using namespace MxEngine;

    class MxApplication : public Application
    {
    public:
        virtual void OnCreate() override
        {
            auto skyboxCubemap = AssetManager::LoadCubeMap("Resources/bluesky.png");

            auto leftEyeObject  = MxObject::Create();
            leftEyeObject->Name = "Left Eye";
            auto leftEyeSkybox = leftEyeObject->AddComponent<Skybox>();
            leftEyeSkybox->Texture = skyboxCubemap;
            auto leftEye = leftEyeObject->AddComponent<CameraController>();
            leftEye->GetCamera<PerspectiveCamera>().SetFOV(90.0f);

            auto rightEyeObject = MxObject::Create();
            rightEyeObject->Name = "Right Eye";
            auto rightEyeSkybox = rightEyeObject->AddComponent<Skybox>();
            rightEyeSkybox->Texture = skyboxCubemap;
            auto rightEye = rightEyeObject->AddComponent<CameraController>();
            rightEye->GetCamera<PerspectiveCamera>().SetFOV(90.0f);

            auto cameraObject = MxObject::Create();
            cameraObject->Name = "VR Camera";

            auto controller = cameraObject->AddComponent<CameraController>();
            controller->SetMoveSpeed(5.0f);
            controller->SetRotateSpeed(5.0f);

            auto input = cameraObject->AddComponent<InputControl>();
            input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
            input->BindRotation();

            auto VR = cameraObject->AddComponent<VRCameraController>();
            VR->LeftEye = leftEye;
            VR->RightEye = rightEye;

            RenderManager::SetViewport(controller);

            auto cubeObject = MxObject::Create();
            cubeObject->Name = "Cube";
            cubeObject->Transform->Translate(MakeVector3(-1.0f, -1.0f, 3.0f));
            cubeObject->AddComponent<MeshSource>(Primitives::CreateCube());
            auto cubeMaterials = cubeObject->AddComponent<MeshRenderer>();
            cubeMaterials->GetMaterial()->AmbientColor = MakeVector3(1.0f, 0.0f, 0.0f);
            cubeMaterials->GetMaterial()->DiffuseColor = MakeVector3(1.0f, 0.0f, 0.0f);

            auto sphereObject = MxObject::Create();
            sphereObject->Name = "Sphere";
            sphereObject->Transform->Translate(MakeVector3(5.0f, 0.0f, 5.0f));
            sphereObject->AddComponent<MeshSource>(Primitives::CreateSphere());
            auto sphereMaterials = sphereObject->AddComponent<MeshRenderer>();
            sphereMaterials->GetMaterial()->AmbientColor = MakeVector3(0.2f, 0.0f, 0.8f);
            sphereMaterials->GetMaterial()->DiffuseColor = MakeVector3(0.2f, 0.0f, 0.8f);

            auto lightObject = MxObject::Create();
            lightObject->Name = "Global Light";
            auto dirLight = lightObject->AddComponent<DirectionalLight>();
            dirLight->Direction = MakeVector3(0.5f, 1.0f, 1.0f);
            dirLight->FollowViewport();
        }

        virtual void OnUpdate() override
        {

        }

        virtual void OnDestroy() override
        {

        }
    };
}

int main()
{
    ProjectTemplate::MxApplication app;
    app.Run();
    return 0;
}