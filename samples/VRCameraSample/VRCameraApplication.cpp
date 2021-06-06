#include <MxEngine.h>

namespace VRCameraSample
{
    using namespace MxEngine;

    /*
    this sample shows how to create working VR camera (render two images as eyes)
    you can set eye distance or focus distance for VR camera. If you want to make focus distance dynamic,
    you need to add colliders for each cube and use raycast from eye center
    for more info about colliders and raycasting, see PhysicsSample
    */
    class VRCameraApplication : public Application
    {
        MxObject::Handle VRCamera;
    public:
        virtual void OnCreate() override
        {
            auto skyboxCubemap = AssetManager::LoadCubeMap("Resources/bluesky.png");

            // create camera for left eye
            auto leftEyeObject = MxObject::Create();
            leftEyeObject->Name = "Left Eye";
            auto leftEyeSkybox = leftEyeObject->AddComponent<Skybox>();
            leftEyeSkybox->CubeMap = skyboxCubemap;
            auto leftEye = leftEyeObject->AddComponent<CameraController>();
            leftEye->GetCamera<PerspectiveCamera>().SetFOV(90.0f);

            // create camera for right eye
            auto rightEyeObject = MxObject::Create();
            rightEyeObject->Name = "Right Eye";
            auto rightEyeSkybox = rightEyeObject->AddComponent<Skybox>();
            rightEyeSkybox->CubeMap = skyboxCubemap;
            auto rightEye = rightEyeObject->AddComponent<CameraController>();
            rightEye->GetCamera<PerspectiveCamera>().SetFOV(90.0f);

            // create VR camera and add other cameras as two eyes
            VRCamera = MxObject::Create();
            VRCamera->Name = "VR Camera";

            auto controller = VRCamera->AddComponent<CameraController>();
            controller->SetMoveSpeed(5.0f);
            controller->SetRotateSpeed(5.0f);

            auto input = VRCamera->AddComponent<InputController>();
            input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
            input->BindRotation();

            auto VR = VRCamera->AddComponent<VRCameraController>();
            VR->LeftEye = leftEye;
            VR->RightEye = rightEye;

            Rendering::SetViewport(controller);

            // create bunch of random cubes to test how VR works with different parameters
            auto cubeObject = MxObject::Create();
            cubeObject->Name = "CubeFactory";
            cubeObject->LocalTransform.Translate(MakeVector3(5.0f, 0.0f, 5.0f));
            cubeObject->AddComponent<MeshSource>(Primitives::CreateCube());
            auto sphereMaterials = cubeObject->AddComponent<MeshRenderer>();
            auto instanceFactory = cubeObject->AddComponent<InstanceFactory>();
            sphereMaterials->GetMaterial()->RoughnessFactor = 0.4f;

            for (size_t i = 0; i < 500; i++)
            {
                auto instance = instanceFactory->Instanciate();

                float rx = Random::GetFloat();
                float ry = Random::GetFloat();
                float rz = Random::GetFloat();

                instance->LocalTransform.Translate(20.0f * MakeVector3(rx, ry, rz) - 10.0f);
                instance->GetComponent<Instance>()->SetColor(MakeVector3(rx, ry, rz));
            }

            // create global light
            auto lightObject = MxObject::Create();
            lightObject->Name = "Global Light";
            auto dirLight = lightObject->AddComponent<DirectionalLight>();
            dirLight->Direction = MakeVector3(0.5f, 1.0f, 1.0f);
            dirLight->SetIntensity(0.5f);
            dirLight->IsFollowingViewport = true;
        }

        virtual void OnUpdate() override { }

        virtual void OnDestroy() override { }
    };
}

int main()
{
    MxEngine::LaunchFromSourceDirectory();
    VRCameraSample::VRCameraApplication app;
    app.Run();
    return 0;
}