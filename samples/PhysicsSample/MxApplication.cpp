#pragma once

#include <MxEngine.h>
#include "Platform/Modules/PhysicsModule.h"
#include "Core/Application/PhysicsManager.h"

namespace ProjectTemplate
{
    using namespace MxEngine;


    class MxApplication : public Application
    {
    public:
        MxObject::Handle cameraObject;
        InstanceFactory::Handle ObjectFactory;
        InstanceFactory::Handle ShotFactory;
        int lenA = 3;
        int lenB = 7;
        int lenC = 3;            
        float BigCubeSize = 160;
        float BigCubeThickness = 8;
        bool PhysicsDebug = false;
        MxObject::Handle UserGunSphere;

        void Reset()
        {
            ObjectFactory->DestroyInstances();
            ShotFactory->DestroyInstances();
            for (size_t i = 0; i < lenA * lenB * lenC; i++)
            {
                float size = 5.f;

                float x = i / (lenC * lenB) % lenA * size;
                float y = i / lenC % lenB * size;
                float z = i % lenC * size;

                float offset = size * 0.5f + BigCubeThickness * 0.5f;

                auto object = ObjectFactory->MakeInstance();

                object->Transform.SetPosition(MakeVector3(x * 1.02, y * 1.004 + offset, z * 1.02));
                object->GetComponent<Instance>()->SetColor(Vector3(x / lenA / size, y / lenB / size, z / lenC / size));
                object->Transform.SetScale(size);

                object->AddComponent<BoxCollider>();
                auto rigidBody = object->AddComponent<RigidBody>();
                rigidBody->SetFriction(0.9);
                rigidBody->SetSpinningFriction(0.9);
                rigidBody->SetBounceFactor(0.1);
                rigidBody->SetMass(1.0f / (0.3f + y * y * y));

                if (PhysicsDebug)
                {
                    object->AddComponent<DebugDraw>()->RenderPhysicsCollider = true;
                }
            }
        }

        void InitializeBoxFace(const Vector3& coord, const Vector3& xyz, const Vector3& offset, const float BigCubeSize, const float thickness, const float visible)
        {
            auto cube = MxObject::Create();
            cube->Name = "Big Cube";
            cube->AddComponent<MeshRenderer>()->GetMaterial()->Transparency = visible;
            cube->AddComponent<MeshSource>(Primitives::CreateCube());
            cube->AddComponent<BoxCollider>();
            auto rigBody = cube->AddComponent<RigidBody>();
            rigBody->SetFriction(0.6);
            rigBody->SetBounceFactor(0.1);
            cube->Transform.SetScale(Vector3(xyz.x * BigCubeSize + thickness, xyz.y * BigCubeSize + thickness, xyz.z * BigCubeSize + thickness));
            cube->Transform.SetPosition(Vector3((BigCubeSize * offset).x / 2, (BigCubeSize * offset).y / 2, (BigCubeSize * offset).z / 2) + coord);
        }

        virtual void OnCreate() override
        {
            cameraObject = MxObject::Create();
            cameraObject->Name = "Player Camera";
            cameraObject->AddComponent<Skybox>()->Texture = AssetManager::LoadCubeMap("bluesky.png"_id);
            cameraObject->Transform.SetPosition(Vector3(30, 30, 30));
            auto controller = cameraObject->AddComponent<CameraController>();
            controller->SetMoveSpeed(50);
            auto input = cameraObject->AddComponent<InputControl>();
            controller->ListenWindowResizeEvent();
            controller->SetMoveSpeed(10.0f);
            input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
            input->BindRotation();
            RenderManager::SetViewport(controller);
            controller->SetMoveSpeed(30);
            // create global directional light
            auto lightObject = MxObject::Create();
            lightObject->Name = "Global Light";
            auto dirLight = lightObject->AddComponent<DirectionalLight>();
            dirLight->ProjectionSize = 150.0f;
            dirLight->FollowViewport();

            PhysicsManager::SetGravity(Vector3(0, -28, 0));

            auto instances = MxObject::Create();
            instances->Name = "Cube Instances";
            instances->AddComponent<MeshSource>(Primitives::CreateCube());
            instances->AddComponent<MeshRenderer>();
            ObjectFactory = instances->AddComponent<InstanceFactory>();

            auto shots = MxObject::Create();
            shots->Name = "Shots Instances";
            shots->AddComponent<MeshSource>(Primitives::CreateSphere());
            auto mat = shots->AddComponent<MeshRenderer>()->GetMaterial();

            auto BallPath = "Ball14.jpg"_id;
            mat->Reflection = 0.5;
            mat->AmbientMap = AssetManager::LoadTexture(BallPath);
            mat->DiffuseMap = AssetManager::LoadTexture(BallPath);
            ShotFactory = shots->AddComponent<InstanceFactory>();

            Vector3 coordOffset(0, BigCubeSize / 2, 0);
            InitializeBoxFace(coordOffset, Vector3(1, 0, 1), Vector3(0, -1, 0), BigCubeSize, BigCubeThickness, 0.6);
            //InitializeBoxFace(coordOffset, Vector3(1, 0, 1), Vector3(0, 1, 0),  cubeSIze, thickness, 0.8);
            InitializeBoxFace(coordOffset, Vector3(0, 1, 1), Vector3(-1, 0, 0), BigCubeSize, BigCubeThickness, 0.6);
            InitializeBoxFace(coordOffset, Vector3(0, 1, 1), Vector3(1, 0, 0), BigCubeSize, BigCubeThickness, 0.25);
            InitializeBoxFace(coordOffset, Vector3(1, 1, 0), Vector3(0, 0, -1), BigCubeSize, BigCubeThickness, 0.6);
            InitializeBoxFace(coordOffset, Vector3(1, 1, 0), Vector3(0, 0, 1), BigCubeSize, BigCubeThickness, 0.6);

            this->Reset();
            PhysicsManager::SetSimulationStep(0.0f); // ban auto-physics sim

            UserGunSphere = MxObject::Create();
            UserGunSphere->AddComponent<MeshSource>()->Mesh = Primitives::CreateSphere();
            UserGunSphere->Transform.SetScale(0.05);
            auto matUserG = UserGunSphere->AddComponent<MeshRenderer>()->GetMaterial();
            matUserG->AmbientMap = AssetManager::LoadTexture(BallPath);
            matUserG->DiffuseMap = AssetManager::LoadTexture(BallPath);
        }

        void Typhoon()
        {
            for (auto& cube : ObjectFactory->GetInstances())
            {
                auto coord = cube->Transform.GetPosition();
                auto velo = Length(coord) * Normalize(Cross(Vector3(0, 1, 0), coord));
                velo.y += Length(coord) / 12;
                auto dist = Length(Vector3(coord.x, 0, coord.z));
                velo.y *= 1 / (1 + pow(1.6, coord.y - dist * 4));
                cube->GetComponent<RigidBody>()->SetLinearVelocity(velo);
            }
        }

        float timeGone = 0;
        virtual void OnUpdate() override
        {
            UserGunSphere->Transform.SetPosition(cameraObject->Transform.GetPosition() + cameraObject->GetComponent<CameraController>()->GetDirection() * (3 - timeGone));
            if (InputManager::IsMouseHeld(MouseButton::LEFT))
            {
                timeGone = Min(timeGone + GetTimeDelta(), 2.5f);
            }
            if (InputManager::IsMouseReleased(MouseButton::LEFT))
            {
                float cubeSize = 1.5f;
                auto object = ShotFactory->MakeInstance();

                if (PhysicsDebug)
                {
                    object->AddComponent<DebugDraw>()->RenderPhysicsCollider = true;
                }

                auto dir = cameraObject->GetComponent<CameraController>()->GetDirection();
                object->Transform.SetScale(cubeSize);
                object->Transform.SetPosition(cameraObject->Transform.GetPosition());
                auto rigidBody = object->AddComponent<RigidBody>();
                object->AddComponent<SphereCollider>();
                rigidBody->SetFriction(0.5);
                rigidBody->SetBounceFactor(2.8);                
                //rigidBody->SetLinearVelocity(dir * (float)((timeGone + 0.1) * 60));
                rigidBody->SetLinearVelocity(dir * (float)((timeGone + 0.0) * 60));
                rigidBody->SetMass(38.0f);
                
                timeGone = 0;
            }

            if (!InputManager::IsMouseHeld(MouseButton::RIGHT))
            {
                PhysicsManager::PerformExtraSimulationStep(1.0f / 60.0f);
            }

            if (InputManager::IsKeyHeld(KeyCode::R))
            {
                Typhoon();
            }

            if (RuntimeManager::IsEditorActive())
            {
                ImGui::Begin("physics");

                if (ImGui::Button("reset"))
                    this->Reset();

                ImGui::Checkbox("debug physics", &PhysicsDebug);

                ImGui::InputInt("X", &lenA);
                ImGui::InputInt("Y", &lenB);
                ImGui::InputInt("Z", &lenC);

                ImGui::End();
            }
        }
    };
}

int main()
{
    ProjectTemplate::MxApplication app;
    app.Run();
    return 0;
}