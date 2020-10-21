#include <MxEngine.h>

#include <iostream>
#include <Utilities\LODGenerator\LODGenerator.h>
#include <House.h>


namespace ProjectTemplate
{
    using namespace MxEngine;

    class MxApplication : public Application
    {
    public:

        virtual void OnCreate() override
        {
            // create camera object
            auto cameraObject = MxObject::Create();
            cameraObject->Name = "Camera Object";
            cameraObject->AddComponent<CameraEffects>();
            cameraObject->AddComponent<CameraToneMapping>();
            Rendering::SetFogDensity(0.02f);
            cameraObject->AddComponent<Skybox>()->Texture = AssetManager::LoadCubeMap("skybox.png"_id);
            // add CameraController component which handles camera image rendering
            auto controller = cameraObject->AddComponent<CameraController>();

            cameraObject->AddComponent<CylinderCollider>()->SetBoundingCylinder(Cylinder(1.5f, 0.45f, 0.45f, Cylinder::Axis::Y));
            cameraObject->AddComponent<RigidBody>();
            auto character = cameraObject->AddComponent<CharacterController>();
            character->SetJumpSpeed(4.0f);
            character->SetJumpPower(11.5f);
            character->SetMass(4);
            
            controller->SetMoveSpeed(8.f);

            // add InpitControl which handles keyboard and mouse input events
            auto input = cameraObject->AddComponent<InputController>();
            // set camera to change ratio automatically depending on application window size
            controller->ListenWindowResizeEvent();
            // bind player movement to classic WASD mode and space/shift to fly, rotation is done with mouse
            input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
            input->BindRotation();
            // set controller to be main application viewport
            Rendering::SetViewport(controller);

            // create global directional light
            auto lightObject = MxObject::Create();
            lightObject->Name = "Global Light";
            // add DirectionalLight component with custom light direction
            auto dirLight = lightObject->AddComponent<DirectionalLight>();
            dirLight->Direction     = MakeVector3(0.5f, 1.0f, 1.0f);
            // make directional light to be centered at current viewport position (is set by RenderManager::SetViewport)
            dirLight->FollowViewport();

            auto cubeFactory = MxObject::Create();
            cubeFactory->Name = "Factory of cubes";
            cubeFactory->AddComponent<InstanceFactory>();
            cubeFactory->AddComponent<MeshSource>(Primitives::CreateCube());
            auto& mat = cubeFactory->AddComponent<MeshRenderer>()->GetMaterial();
            mat->AlbedoMap = AssetManager::LoadTexture("brick.jpg"_id);
            mat->NormalMap = AssetManager::LoadTexture("brick_normal.jpg"_id);
            mat->AmbientOcclusionMap = AssetManager::LoadTexture("brick_ao.jpg"_id);

            shotFactory = MxObject::Create();
            shotFactory->Name = "Factory of shots";
            shotFactory->AddComponent<InstanceFactory>();
            shotFactory->AddComponent<MeshSource>(Primitives::CreateSphere());
            shotFactory->AddComponent<MeshRenderer>()->GetMaterial()->Emmision = 3.5f;

            auto ground = MxObject::Create();
            ground->Transform.SetPosition(Vector3(0, -4, 0));
            
            ground->AddComponent<MeshSource>(Primitives::CreateCube());
            ground->AddComponent<BoxCollider>();
            ground->AddComponent<RigidBody>();
            ground->Transform.SetScale(Vector3(500, 5.f, 500));

            auto groundWithTexture = MxObject::Create();
            groundWithTexture->Transform.SetPosition(Vector3(0, 2.5f - 4.f + 0.01f, 0));
            groundWithTexture->AddComponent<MeshSource>(Primitives::CreatePlane(500));
            auto& matWithTexture = groundWithTexture->AddComponent<MeshRenderer>()->GetMaterial();
            matWithTexture->AlbedoMap = AssetManager::LoadTexture("field.png"_id);
            matWithTexture->SpecularFactor = 0;

            camera = cameraObject;

            auto& ctor = [cubeFactory, cameraObject](auto& coords, float angular)
            {
                auto cube = cubeFactory->GetComponent<InstanceFactory>()->MakeInstance();
                cube->AddComponent<BoxCollider>();
                auto controller = cameraObject->GetComponent<CameraController>();

                cube->Transform.SetPosition(coords);

                auto rigidBody = cube->AddComponent<RigidBody>();
                rigidBody->MakeDynamic();
                rigidBody->SetFriction(100);
                rigidBody->SetAngularForceFactor(Vector3(angular));
                rigidBody->SetMass(40);

                cube->Transform.SetScale(0.999f);
            };

            BuildHouse(
                VectorInt3(10, 10, 10),
                VectorInt3(10, 30, 10),
                [ctor](auto& coords) { ctor(coords, 0); }
            );

            BuildPyramid(10, [ctor](auto& coords) { ctor(coords + Vector3(20, 0, 20), 0.03f); });

            this->cubeFactory = cubeFactory;
        }

        MxObject::Handle camera;
        MxObject::Handle cubeFactory;
        MxObject::Handle shotFactory;

        virtual void OnUpdate() override
        {
            if (Input::IsMousePressed(MouseButton::LEFT))
            {
                auto cube = cubeFactory->GetComponent<InstanceFactory>()->MakeInstance();
                cube->AddComponent<BoxCollider>();
                cube->GetComponent<Instance>()->SetColor(0.5f * Random::GetUnitVector3() + 0.5f);
                auto controller = camera->GetComponent<CameraController>();

                cube->Transform.SetPosition(camera->Transform.GetPosition());

                auto rigidBody = cube->AddComponent<RigidBody>();
                rigidBody->MakeDynamic();
                rigidBody->SetLinearVelocity(controller->GetDirection() * 5.0f);
                rigidBody->SetFriction(10);
                rigidBody->SetAngularForceFactor(Vector3(0.0f));
                rigidBody->SetMass(8);
            }
            else if (Input::IsMousePressed(MouseButton::RIGHT))
            {
                auto shot = shotFactory->GetComponent<InstanceFactory>()->MakeInstance();
                shot->AddComponent<BoxCollider>();
                auto color = 0.5f * Random::GetUnitVector3() + 0.5f;
                shot->GetComponent<Instance>()->SetColor(color);
                auto controller = camera->GetComponent<CameraController>();

                shot->Transform.SetPosition(camera->Transform.GetPosition() + controller->GetDirection() * 1.f);
                shot->Transform.SetScale(0.4f);
                auto pointLight = shot->AddComponent<PointLight>();
                pointLight->ToggleShadowCast(false);
                pointLight->AmbientColor = color * 0.7f;
                pointLight->DiffuseColor = color;
                pointLight->SpecularColor = color;
                pointLight->UseRadius(1);
                

                auto rigidBody = shot->AddComponent<RigidBody>();
                rigidBody->MakeDynamic();
                rigidBody->SetLinearVelocity(controller->GetDirection() * 25.0f);
                rigidBody->SetMass(10);
                rigidBody->SetBounceFactor(5);

                Timer::CallAfterDelta([shot]() mutable { MxObject::Destroy(shot); }, 10.f);
            }
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