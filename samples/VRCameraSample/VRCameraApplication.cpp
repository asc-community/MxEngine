#include <MxEngine.h>

namespace VRCameraSample
{
    using namespace MxEngine;

    class VRCameraApplication : public Application
    {
        MxObject::Handle VRCamera;
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

            VRCamera = MxObject::Create();
            VRCamera->Name = "VR Camera";

            auto controller = VRCamera->AddComponent<CameraController>();
            controller->SetMoveSpeed(5.0f);
            controller->SetRotateSpeed(5.0f);

            auto input = VRCamera->AddComponent<InputControl>();
            input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
            input->BindRotation();

            auto VR = VRCamera->AddComponent<VRCameraController>();
            VR->LeftEye = leftEye;
            VR->RightEye = rightEye;

            Rendering::SetViewport(controller);

            auto cubeObject = MxObject::Create();
            cubeObject->Name = "Cube";
            cubeObject->Transform.Translate(MakeVector3(-1.0f, -1.0f, 3.0f));
            cubeObject->AddComponent<MeshSource>(Primitives::CreateCube());
            auto cubeMaterials = cubeObject->AddComponent<MeshRenderer>();
            cubeMaterials->GetMaterial()->AmbientColor = MakeVector3(1.0f, 0.0f, 0.0f);
            cubeMaterials->GetMaterial()->DiffuseColor = MakeVector3(1.0f, 0.0f, 0.0f);

            auto sphereObject = MxObject::Create();
            sphereObject->Name = "Sphere";
            sphereObject->Transform.Translate(MakeVector3(5.0f, 0.0f, 5.0f));
            sphereObject->AddComponent<MeshSource>(Primitives::CreateSphere());
            auto sphereMaterials = sphereObject->AddComponent<MeshRenderer>();
            auto instanceFactory = sphereObject->AddComponent<InstanceFactory>();
            
            for (size_t i = 0; i < 20; i++)
            {
                auto sphereInstance = instanceFactory->MakeInstance();

                float rx = Random::GetFloat();
                float ry = Random::GetFloat();
                float rz = Random::GetFloat();

                sphereInstance->Transform.Translate(20.0f * MakeVector3(rx, ry, rz) - 10.0f);
                sphereInstance->GetComponent<Instance>()->SetColor(MakeVector3(rx, ry, rz));
            }

            auto lightObject = MxObject::Create();
            lightObject->Name = "Global Light";
            auto dirLight = lightObject->AddComponent<DirectionalLight>();
            dirLight->Direction = MakeVector3(0.5f, 1.0f, 1.0f);
            dirLight->FollowViewport();
        }

        virtual void OnUpdate() override
        {
            if (this->VRCamera.IsValid() && this->VRCamera->HasComponent<VRCameraController>())
            {
                auto& vr = *VRCamera->GetComponent<VRCameraController>();
                ImGui::Begin("VR info");
                ImGui::Text("eye focus distance: %f", vr.FocusDistance);
                ImGui::Text("eye distance: %f", vr.EyeDistance);
                ImGui::Text(" left eye fov: %f",  vr.LeftEye->GetCamera<PerspectiveCamera>().GetFOV());
                ImGui::Text("right eye fov: %f", vr.RightEye->GetCamera<PerspectiveCamera>().GetFOV());
                ImGui::End();
            }
        }

        virtual void OnDestroy() override
        {

        }
    };
}

int main()
{
    VRCameraSample::VRCameraApplication app;
    app.Run();
    return 0;
}