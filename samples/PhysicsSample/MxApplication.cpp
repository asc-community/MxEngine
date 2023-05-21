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
        MxObject::Handle player;
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

                auto object = physicalObjectFactory->Instanciate();
                object->Name = "Cube Instance";

                object->LocalTransform.SetPosition(Vector3(x, y + offset, z));
                object->GetComponent<Instance>()->SetColor(Vector3(x / cubeConstraintsA / size, y / cubeConstraintsB / size, z / cubeConstraintsC / size));
                object->LocalTransform.SetScale(size);

                // create rigid body with box collider. setup extra parameters to make objects more balanced
                object->AddComponent<BoxCollider>();
                auto rigidBody = object->AddComponent<RigidBody>();
                rigidBody->MakeDynamic();
                rigidBody->SetMass(100.0f);
                rigidBody->SetAngularForceFactor(Vector3(0.1f));
                rigidBody->SetActivationState(ActivationState::ISLAND_SLEEPING);

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
            auto material = wall->AddComponent<MeshRenderer>()->GetMaterial();
            material->Transparency = transparency;
            wall->LocalTransform.SetScale(Vector3(xyz.x * size + thickness, xyz.y * size + thickness, xyz.z * size + thickness));
            wall->LocalTransform.SetPosition(Vector3((size * offset).x / 2, (size * offset).y / 2, (size * offset).z / 2) + coord);
            wall->AddComponent<BoxCollider>();
            wall->AddComponent<RigidBody>();
        }

        void CreateShot()
        {
            auto object = shotFactory->Instanciate();
            object->Name = "Bullet Instance";

            if (debugPhysics)
            {
                object->AddComponent<DebugDraw>()->RenderPhysicsCollider = true;
            }

            auto dir = player->GetComponent<CameraController>()->GetDirection();
            object->LocalTransform.SetScale(shotSize);
            object->LocalTransform.SetPosition(player->LocalTransform.GetPosition() + 5.0f * dir);
            object->AddComponent<SphereCollider>();
            auto rigidBody = object->AddComponent<RigidBody>();
            rigidBody->MakeDynamic();
            rigidBody->SetLinearVelocity(dir * 180.0f);
            rigidBody->SetMass(50.0f);
            rigidBody->SetBounceFactor(0.5f);

            Timer::CallAfterDelta([object]() mutable { MxObject::Destroy(object); }, 10.0f);
        }

        void InitializePlayer()
        {
            player = MxObject::Create();
            player->Name = "Player";
            player->AddComponent<CameraToneMapping>();
            player->LocalTransform.SetPosition(Vector3(30, 30, 30));

            auto skybox = player->AddComponent<Skybox>();
            skybox->CubeMap = AssetManager::LoadCubeMap("Resources/dawn.jpg"_id);
            skybox->Irradiance = AssetManager::LoadCubeMap("Resources/dawn_irradiance.jpg"_id);
            
            auto controller = player->AddComponent<CameraController>();
            controller->ListenWindowResizeEvent();
            Rendering::SetViewport(controller);

            auto input = player->AddComponent<InputController>();
            input->BindMovementWASDSpaceShift();
            input->BindRotation();
            input->SetMoveSpeed(30.0f);

            auto collider = player->AddComponent<CapsuleCollider>();
            collider->SetBoundingCapsule(Capsule(5.0f, 5.0f, Capsule::Axis::Y));

            auto rigidBody = player->AddComponent<RigidBody>();
            rigidBody->SetMass(100.0f);

            auto characterController = player->AddComponent<CharacterController>();
            characterController->SetJumpPower(20.0f);
            characterController->SetJumpSpeed(10.0f);
        }

        void CreateCubeFactory()
        {
            // create factories for physical objects and player shots
            auto instances = MxObject::Create();
            instances->Name = "Cube Instances";
            instances->AddComponent<MeshSource>(Primitives::CreateCube());
            auto cubesMaterial = instances->AddComponent<MeshRenderer>()->GetMaterial();
            cubesMaterial->RoughnessFactor = 0.15f;
            cubesMaterial->MetallicFactor = 1.0f;
            physicalObjectFactory = instances->AddComponent<InstanceFactory>();
        }

        void AddTrigger()
        {
            auto object = MxObject::Create();
            object->Name = "Trigger";
            object->LocalTransform.SetPosition(Vector3(40.0f, 15.0f, -40.0f));
            object->LocalTransform.SetScale(30.0f);

            auto mr = object->AddComponent<MeshRenderer>();
            auto ms = object->AddComponent<MeshSource>(Primitives::CreateSphere());
            auto rb = object->AddComponent<RigidBody>();
            auto cl = object->AddComponent<SphereCollider>();

            auto material = mr->GetMaterial();
            material->BaseColor = Colors::Create(Colors::GREEN);
            material->Transparency = 0.3f;
            material->MetallicFactor = 1.0f;
            material->RoughnessFactor = 0.4f;
            rb->MakeTrigger();
            rb->SetOnCollisionEnterCallback([](MxObject::Handle self, MxObject::Handle other)
            {
                Logger::Log(VerbosityType::INFO, self->Name, "enter collision with: " + other->Name);
                if (other->Name == "Cube Instance")
                    MxObject::Destroy(other);
            });
            rb->SetOnCollisionExitCallback([](MxObject::Handle self, MxObject::Handle other)
            {
                Logger::Log(VerbosityType::INFO, self->Name, "exit collision with: " + other->Name);
            });
        }

    public:
        virtual void OnCreate() override
        {
            this->InitializePlayer();

            // create global directional light
            auto lightObject = MxObject::Create();
            lightObject->Name = "Global Light";
            auto dirLight = lightObject->AddComponent<DirectionalLight>();
            dirLight->Direction = MakeVector3(0.1f, 1.0f, 0.0f);
            dirLight->IsFollowingViewport = true;
            dirLight->Projections[0] = 50.0f;
            dirLight->Projections[1] = 200.0f;

            this->CreateCubeFactory();
            
            auto shots = MxObject::Create();
            shots->Name = "Bullet Instances";
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

            this->AddTrigger();

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
                auto dir = player->GetComponent<CameraController>()->GetDirection();
                auto pos = player->LocalTransform.GetPosition();
                auto end = pos + dir * 1000.0f;
                float fraction = 0.0f;
                auto lookingAt = Physics::RayCast(pos, end, fraction);
                float distance = Length(end - pos) * fraction;
                auto gravity = Physics::GetGravity();

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

                if (ImGui::DragFloat3("gravity", &gravity[0], 0.01f, -10000.0f, 10000.0f))
                    Physics::SetGravity(gravity);

                cubeConstraintsA = Max(0, x), cubeConstraintsB = Max(0, y), cubeConstraintsC = Max(0, z);

                ImGui::End();
            }
        }
    };
}

int main()
{
    MxEngine::LaunchFromSourceDirectory();
    PhysicsSample::PhysicsApplication app;
    app.Run();
    return 0;
}