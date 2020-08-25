#include <MxEngine.h>

namespace PhysicsSample
{
    using namespace MxEngine;

    /*
    this big sample shows how engine physics and raycasting work
    here we create big box with open top and put some physical objects inside
    player allowed to shoot spheres to interact with the environment
    many parameters can be set in runtime editor: 
    reseting simulation, dynamic object count, debug draw for physic colliders
    also, when editor is enabled, raycasting info is shown and raycasted point is indicated by small red box
    */
    class PhysicsApplication : public Application
    {
        MxObject::Handle cameraObject;
        InstanceFactory::Handle physicalObjectFactory;
        InstanceFactory::Handle shotFactory;
        bool debugPhysics = false;
        size_t cubeConstraintsA = 3;
        size_t cubeConstraintsB = 7;
        size_t cubeConstraintsC = 3;

        const float wallSize = 160.0f;
        const float wallThickness = 3.0f;
        const float shotSize = 3.0f;

        void ResetSimulation()
        {
            physicalObjectFactory->DestroyInstances();
            shotFactory->DestroyInstances();

            for (size_t i = 0; i < cubeConstraintsA * cubeConstraintsB * cubeConstraintsC; i++)
            {
                const float size = 5.0f;
                const float offset = size * 0.5f + wallThickness * 0.5f;

                float x = i / (cubeConstraintsC * cubeConstraintsB) % cubeConstraintsA * size; //-V104 //-V636
                float y = i / cubeConstraintsC % cubeConstraintsB * size;                      //-V104 //-V636
                float z = i % cubeConstraintsC * size;                                         //-V104 //-V636

                auto object = physicalObjectFactory->MakeInstance();

                object->Transform.SetPosition(Vector3(x, y + offset, z));
                object->GetComponent<Instance>()->SetColor(Vector3(x / cubeConstraintsA / size, y / cubeConstraintsB / size, z / cubeConstraintsC / size));
                object->Transform.SetScale(size);

                // create rigid body with box collider. setup extra parameters to make objects more balanced
                object->AddComponent<BoxCollider>();
                auto rigidBody = object->AddComponent<RigidBody>();
                rigidBody->MakeDynamic();
                rigidBody->SetMass(800.0f / (1.0f + y * y * y));
                rigidBody->SetAngularForceFactor(Vector3(0.01f));

                if (debugPhysics)
                {
                    object->AddComponent<DebugDraw>()->RenderPhysicsCollider = true;
                }
            }
        }

        void InitializeWall(const Vector3& coord, const Vector3& xyz, const Vector3& offset, const float size, const float thickness, const float transparency)
        {
            auto wall = MxObject::Create();
            wall->Name = "Wall";
            wall->AddComponent<MeshSource>(Primitives::CreateCube());
            wall->AddComponent<BoxCollider>();
            wall->AddComponent<RigidBody>();
            auto material = wall->AddComponent<MeshRenderer>()->GetMaterial();
            material->Transparency = transparency;
            wall->Transform.SetScale(Vector3(xyz.x * size + thickness, xyz.y * size + thickness, xyz.z * size + thickness));
            wall->Transform.SetPosition(Vector3((size * offset).x / 2, (size * offset).y / 2, (size * offset).z / 2) + coord);
        }

        void CreateShot()
        {
            auto object = shotFactory->MakeInstance();

            if (debugPhysics)
            {
                object->AddComponent<DebugDraw>()->RenderPhysicsCollider = true;
            }

            auto dir = cameraObject->GetComponent<CameraController>()->GetDirection();
            object->Transform.SetScale(shotSize);
            object->Transform.SetPosition(cameraObject->Transform.GetPosition());
            object->AddComponent<SphereCollider>();
            auto rigidBody = object->AddComponent<RigidBody>();
            rigidBody->MakeDynamic();
            rigidBody->SetLinearVelocity(dir * 180.0f);
            rigidBody->SetMass(50.0f);
        }

    public:
        virtual void OnCreate() override
        {
            // setup camera
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

            // create factories for physical objects and player shots
            auto instances = MxObject::Create();
            instances->Name = "Cube Instances";
            instances->AddComponent<MeshSource>(Primitives::CreateCube());
            instances->AddComponent<MeshRenderer>();
            physicalObjectFactory = instances->AddComponent<InstanceFactory>();

            auto shots = MxObject::Create();
            shots->Name = "Shots Instances";
            shots->AddComponent<MeshSource>(Primitives::CreateSphere());
            shots->AddComponent<MeshRenderer>();
            shotFactory = shots->AddComponent<InstanceFactory>();

            // create open-box shape where simulation will happen
            Vector3 coordinateOffset(0, wallSize / 2, 0);
            InitializeWall(coordinateOffset, Vector3(1, 0, 1), Vector3( 0, -1,  0), wallSize, wallThickness, 0.4f);
            InitializeWall(coordinateOffset, Vector3(0, 1, 1), Vector3(-1,  0,  0), wallSize, wallThickness, 0.4f);
            InitializeWall(coordinateOffset, Vector3(0, 1, 1), Vector3( 1,  0,  0), wallSize, wallThickness, 0.4f);
            InitializeWall(coordinateOffset, Vector3(1, 1, 0), Vector3( 0,  0, -1), wallSize, wallThickness, 0.4f);
            InitializeWall(coordinateOffset, Vector3(1, 1, 0), Vector3( 0,  0,  1), wallSize, wallThickness, 0.4f);

            this->ResetSimulation();
        }

        virtual void OnUpdate() override
        {
            // allow player to shoot every 100ms
            static float timeSinceShot = 0.0f;
            const float shootInterval = 0.1f;
            timeSinceShot += GetTimeDelta();
            if (Input::IsMouseHeld(MouseButton::LEFT) && timeSinceShot > shootInterval)
            {
                timeSinceShot = 0.0f;
                this->CreateShot();
            }

            // draw small red box where player is looking at
            if (Runtime::IsEditorActive())
            {
                auto dir = cameraObject->GetComponent<CameraController>()->GetDirection();
                auto pos = cameraObject->Transform.GetPosition();
                auto end = pos + dir * 1000.0f;
                float fraction = 0.0f;
                auto lookingAt = Physics::RayCast(pos, end, fraction);
                float distance = Length(end - pos) * fraction;

                Rendering::Draw(BoundingBox(pos + dir * distance, MakeVector3(1.0f)), Colors::Create(Colors::RED, 1.0f));

                ImGui::Begin("physics");

                ImGui::Text("raycast");
                if (lookingAt.IsValid())
                {
                    ImGui::Text("distance to object: %f", distance);
                    ImGui::Text("object name: %s", lookingAt->Name.c_str());
                }
                else
                {
                    ImGui::Text("no object");
                }
                ImGui::Separator();

                if (ImGui::Button("reset"))
                {
                    this->ResetSimulation();
                }

                ImGui::Checkbox("debug physics", &debugPhysics);

                auto x = (int)cubeConstraintsA, y = (int)cubeConstraintsB, z = (int)cubeConstraintsC;
                ImGui::InputInt("X", &x);
                ImGui::InputInt("Y", &y);
                ImGui::InputInt("Z", &z);

                cubeConstraintsA = Max(0, x), cubeConstraintsB = Max(0, y), cubeConstraintsC = Max(0, z);

                ImGui::End();
            }
        }
    };
}

int main()
{
    PhysicsSample::PhysicsApplication app;
    app.Run();
    return 0;
}