#pragma once

#include <MxEngine.h>

namespace PhysicsSample
{
    using namespace MxEngine;

    class PhysicsApplication : public Application
    {
    public:
        MxObject::Handle cameraObject;
        InstanceFactory::Handle ObjectFactory;
        InstanceFactory::Handle ShotFactory;
        int lenA = 3;
        int lenB = 7;
        int lenC = 3;            
        float BigCubeSize = 160.0f;
        float BigCubeThickness = 3.0f;
        bool PhysicsDebug = false;

        void Reset()
        {
            ObjectFactory->DestroyInstances();
            ShotFactory->DestroyInstances();
            for (size_t i = 0; i < size_t(lenA * lenB * lenC); i++)
            {
                float size = 5.0f;

                float x = i / (lenC * lenB) % lenA * size; //-V104 //-V636
                float y = i / lenC % lenB * size;          //-V104 //-V636
                float z = i % lenC * size;                 //-V104 //-V636

                float offset = size * 0.5f + BigCubeThickness * 0.5f;

                auto object = ObjectFactory->MakeInstance();

                object->Transform.SetPosition(Vector3(x, y + offset, z));
                object->GetComponent<Instance>()->SetColor(Vector3(x / lenA / size, y / lenB / size, z / lenC / size));
                object->Transform.SetScale(size);

                object->AddComponent<BoxCollider>();
                auto rigidBody = object->AddComponent<RigidBody>();
                rigidBody->SetMass(800.0f / (1.0f + y * y * y));
                rigidBody->SetAngularForceFactor(Vector3(0.01f));

                if (PhysicsDebug)
                {
                    object->AddComponent<DebugDraw>()->RenderPhysicsCollider = true;
                }
            }
        }

        void InitializeBoxFace(const Vector3& coord, const Vector3& xyz, const Vector3& offset, const float BigCubeSize, const float thickness, const float transparency)
        {
            auto cube = MxObject::Create();
            cube->Name = "Big Cube";
            cube->AddComponent<MeshRenderer>()->GetMaterial()->Transparency = transparency;
            cube->AddComponent<MeshSource>(Primitives::CreateCube());
            cube->AddComponent<BoxCollider>();
            cube->AddComponent<RigidBody>();
            cube->Transform.SetScale(Vector3(xyz.x * BigCubeSize + thickness, xyz.y * BigCubeSize + thickness, xyz.z * BigCubeSize + thickness));
            cube->Transform.SetPosition(Vector3((BigCubeSize * offset).x / 2, (BigCubeSize * offset).y / 2, (BigCubeSize * offset).z / 2) + coord);
        }

        void CreateShot()
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
            object->AddComponent<SphereCollider>();
            auto rigidBody = object->AddComponent<RigidBody>();
            rigidBody->SetLinearVelocity(dir * 180.0f);
            rigidBody->SetMass(50.0f);
        }

        virtual void OnCreate() override
        {
            cameraObject = MxObject::Create();
            cameraObject->Name = "Player Camera";
            cameraObject->AddComponent<Skybox>()->Texture = AssetManager::LoadCubeMap("dawn.jpg"_id);
            cameraObject->Transform.SetPosition(Vector3(30, 30, 30));
            auto controller = cameraObject->AddComponent<CameraController>();
            controller->SetMoveSpeed(50);
            auto input = cameraObject->AddComponent<InputControl>();
            controller->ListenWindowResizeEvent();
            controller->SetMoveSpeed(30);
            input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
            input->BindRotation();
            Rendering::SetViewport(controller);
            Rendering::SetFogDensity(0.0f);

            // create global directional light
            auto lightObject = MxObject::Create();
            lightObject->Name = "Global Light";
            auto dirLight = lightObject->AddComponent<DirectionalLight>();
            dirLight->ProjectionSize = 250.0f;
            dirLight->Direction = MakeVector3(0.1f, 1.0f, 0.0f);
            dirLight->FollowViewport();

            auto instances = MxObject::Create();
            instances->Name = "Cube Instances";
            instances->AddComponent<MeshSource>(Primitives::CreateCube());
            instances->AddComponent<MeshRenderer>();
            ObjectFactory = instances->AddComponent<InstanceFactory>();

            auto shots = MxObject::Create();
            shots->Name = "Shots Instances";
            shots->AddComponent<MeshSource>(Primitives::CreateSphere());
            shots->AddComponent<MeshRenderer>();
            ShotFactory = shots->AddComponent<InstanceFactory>();

            Vector3 coordOffset(0, BigCubeSize / 2, 0);
            InitializeBoxFace(coordOffset, Vector3(1, 0, 1), Vector3( 0, -1,  0), BigCubeSize, BigCubeThickness, 0.6f);
            InitializeBoxFace(coordOffset, Vector3(0, 1, 1), Vector3(-1,  0,  0), BigCubeSize, BigCubeThickness, 0.6f);
            InitializeBoxFace(coordOffset, Vector3(0, 1, 1), Vector3( 1,  0,  0), BigCubeSize, BigCubeThickness, 0.6f);
            InitializeBoxFace(coordOffset, Vector3(1, 1, 0), Vector3( 0,  0, -1), BigCubeSize, BigCubeThickness, 0.6f);
            InitializeBoxFace(coordOffset, Vector3(1, 1, 0), Vector3( 0,  0,  1), BigCubeSize, BigCubeThickness, 0.6f);

            this->Reset();
            Physics::SetSimulationStep(0.0f); // disable auto-physics sim
        }

        virtual void OnUpdate() override
        {
            static float timeSinceShot = 0.0f;
            timeSinceShot += GetTimeDelta();
            if (Input::IsMouseHeld(MouseButton::LEFT) && timeSinceShot > 0.1f)
            {
                timeSinceShot = 0.0f;
                this->CreateShot();
            }

            if (!Input::IsMouseHeld(MouseButton::RIGHT))
            {
                Physics::PerformExtraSimulationStep(this->GetTimeDelta());
            }

            if (Runtime::IsEditorActive())
            {
                auto dir = cameraObject->GetComponent<CameraController>()->GetDirection();
                auto pos = cameraObject->Transform.GetPosition();
                float dist = 0.0f;
                auto lookingAt = Physics::RayCast(pos, pos + dir * 1000.0f, dist);

                ImGui::Begin("physics");

                ImGui::Text("raycast");
                if (lookingAt.IsValid())
                {
                    ImGui::Text("distance to object: %f", dist);
                    ImGui::Text("object name: %s", lookingAt->Name.c_str());
                }
                else
                {
                    ImGui::Text("no object");
                }
                ImGui::Separator();

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
    OfflineRenderer::PhysicsApplication app;
    app.Run();
    return 0;
}