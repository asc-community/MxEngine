#pragma once

#include <MxEngine.h>

#include <Vendors/bullet3/btBulletDynamicsCommon.h>

//#define SPHERES_INSTEAD_CUBES

namespace ProjectTemplate
{
    using namespace MxEngine;

    btVector3 btVec2Vec(const Vector3& vec)
    {
        return btVector3(vec.x, vec.y, vec.z);
    }

    class MxApplication : public Application
    {
    public:
        UniqueRef<btCollisionConfiguration> CollisionConfiguration;
        UniqueRef<btDispatcher> Dispatcher;
        UniqueRef<btBroadphaseInterface>  Broadphase;
        UniqueRef<btConstraintSolver> Solver;
        UniqueRef<btDiscreteDynamicsWorld> World;
        MxVector<std::pair<Ref<btRigidBody>, MxObject::Handle>> Objects;
        MxObject::Handle cameraObject;
        InstanceFactory::Handle ObjectFactory;
        InstanceFactory::Handle ShotFactory;
        int lenA = 7;
        int lenB = 30;
        int lenC = 7;
        btRigidBody* bigCube;
        MxVector<std::pair<Ref<btRigidBody>, MxObject::Handle>> Cubes;

        void Reset()
        {
            this->DestroyPhysics();
            ObjectFactory->DestroyInstances();
            ShotFactory->DestroyInstances();
            Objects.clear();
            Cubes.clear();
            for (size_t i = 0; i < lenA * lenB * lenC; i++)
            {
                float size = 3.f;

                #if defined(SPHERES_INSTEAD_CUBES)
                btCollisionShape* colShape = new btSphereShape(size * 0.5f);
                #else
                btCollisionShape* colShape = new btBoxShape(btVector3(size * 0.5f, size * 0.5f, size * 0.5f));
                #endif

                float x = i / (lenC * lenB) % lenA * size;
                float y = i /  lenC % lenB * size;
                float z = i %  lenC * size;

                float offset = size * 0.5f;

                btTransform startTransform;
                startTransform.setIdentity();
                startTransform.setOrigin(btVector3(x, y + offset, z));

                btScalar mass(1.f / (y * y * y + 1));
                btVector3 localInertia(0, 0, 0);
                colShape->calculateLocalInertia(mass, localInertia);

                //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
                btMotionState* myMotionState = new btDefaultMotionState(startTransform);
                btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

                auto& object = this->Objects.emplace_back();
                object.first = MakeRef<btRigidBody>(rbInfo);
                object.second = ObjectFactory->MakeInstance();
                object.second->GetComponent<Instance>()->SetColor(Vector3(x / lenA / size, y / lenB / size, z / lenC / size));
                object.second->Transform.SetScale(size);
                this->World->addRigidBody(object.first.get());

                std::pair<Ref<btRigidBody>, MxObject::Handle> obj = std::make_pair(object.first, object.second);
                Cubes.push_back(obj);
            }
        }

        void InitializeBoxFace(const Vector3& coord, const VectorInt3& xyz, const Vector3& offset, const float BigCubeSize, const float thickness)
        {
            btCollisionShape* groundShape = new btBoxShape(btVector3(xyz.x * 0.5f * BigCubeSize + thickness, xyz.y * 0.5f * BigCubeSize + thickness, xyz.z * 0.5f * BigCubeSize + thickness));

            btTransform groundTransform;
            groundTransform.setIdentity();
            groundTransform.setOrigin(btVector3((BigCubeSize * offset).x, (BigCubeSize * offset).y, (BigCubeSize * offset).z) / 2 + btVec2Vec(coord));

            btScalar mass(0.0f);
            btVector3 localInertia(0, 0, 0);

            //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
            btMotionState* myMotionState = new btDefaultMotionState(groundTransform);
            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);

            //add the body to the dynamics world

            bigCube = new btRigidBody(rbInfo);


            auto cube = MxObject::Create();
            cube->Name = "Big Cube";
            cube->AddComponent<MeshRenderer>();
            cube->AddComponent<MeshSource>(Primitives::CreateCube());
            cube->Transform.SetScale(Vector3(xyz.x * BigCubeSize + thickness, xyz.y * BigCubeSize + thickness, xyz.z * BigCubeSize + thickness));
            cube->Transform.SetPosition(Vector3((BigCubeSize * offset).x / 2, (BigCubeSize * offset).y / 2, (BigCubeSize * offset).z / 2) + coord);

            this->World->addRigidBody(bigCube);
        }

        virtual void OnCreate() override
        {
            cameraObject = MxObject::Create();
            cameraObject->Name = "Player Camera";
            cameraObject->AddComponent<Skybox>()->Texture = AssetManager::LoadCubeMap(R"(D:\repos\MxEngine\samples\SandboxApplication\Resources\textures\dawn.jpg)");
            cameraObject->Transform.SetPosition(Vector3(30, 30, 30));
            auto controller = cameraObject->AddComponent<CameraController>();
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
            dirLight->Direction = MakeVector3(0.5f, 1.0f, 1.0f);
            dirLight->FollowViewport();

            // set up world
            this->CollisionConfiguration = MakeUnique<btDefaultCollisionConfiguration>();
            this->Dispatcher = MakeUnique<btCollisionDispatcher>(this->CollisionConfiguration.get());
            this->Broadphase = MakeUnique<btDbvtBroadphase>();
            this->Solver = MakeUnique<btSequentialImpulseConstraintSolver>();
            this->World = MakeUnique<btDiscreteDynamicsWorld>(
                this->Dispatcher.get(), this->Broadphase.get(), this->Solver.get(), this->CollisionConfiguration.get());
            this->World->setGravity(btVector3(0.0f, -9.8f, 0.0f));

            auto instances = MxObject::Create();
            #if defined(SPHERES_INSTEAD_CUBES)
            instances->AddComponent<MeshSource>(Primitives::CreateSphere());
            #else
            instances->AddComponent<MeshSource>(Primitives::CreateCube());
            #endif
            instances->AddComponent<MeshRenderer>();
            ObjectFactory = instances->AddComponent<InstanceFactory>();

            auto shots = MxObject::Create();
            shots->AddComponent<MeshSource>(Primitives::CreateSphere());
            shots->AddComponent<MeshRenderer>();
            ShotFactory = shots->AddComponent<InstanceFactory>();

            //this->InitializeBigCube();
            float cubeSIze = 160;
            float thickness = 1.0f;
            Vector3 coordOffset(0, cubeSIze / 2 + thickness, 0);
            InitializeBoxFace(coordOffset, Vector3(1, 0, 1), Vector3(0, -1, 0), cubeSIze, thickness);
            InitializeBoxFace(coordOffset, Vector3(1, 0, 1), Vector3(0, 1, 0),  cubeSIze, thickness);
            InitializeBoxFace(coordOffset, Vector3(0, 1, 1), Vector3(-1, 0, 0), cubeSIze, thickness);
            InitializeBoxFace(coordOffset, Vector3(0, 1, 1), Vector3(1, 0, 0),  cubeSIze, thickness);
            InitializeBoxFace(coordOffset, Vector3(1, 1, 0), Vector3(0, 0, -1), cubeSIze, thickness);
            InitializeBoxFace(coordOffset, Vector3(1, 1, 0), Vector3(0, 0, 1),  cubeSIze, thickness);
            this->Reset();
        }

        void Typhoon()
        {
            for (auto& cube : Cubes)
            {
                auto coord = cube.second->Transform.GetPosition();
                auto velo = 1.3f * Length(coord) * Normalize(Cross(Vector3(0, 1, 0), Normalize(coord)));
                velo.y += Length(coord) / 6;
                cube.first->setLinearVelocity(btVec2Vec(velo));
            }
        }

        float timeGone = 0;
        virtual void OnUpdate() override
        {
            timeGone += GetTimeDelta();
            if (InputManager::IsMouseHeld(MouseButton::LEFT) && timeGone > 0.1)
            {
                float cubeSize = 1.5f;
                timeGone = 0;
                #if defined(SPHERES_INSTEAD_CUBES)
                btCollisionShape* colShape = new btSphereShape(cubeSize * 0.5f);
                #else
                btCollisionShape* colShape = new btBoxShape(btVector3(cubeSize * 0.5f, cubeSize * 0.5f, cubeSize * 0.5f));
                #endif

                btTransform startTransform;
                startTransform.setIdentity();
                startTransform.setOrigin(btVector3(btVec2Vec(cameraObject->Transform.GetPosition())));

                btScalar mass(38.f);
                btVector3 localInertia(0, 0, 0);
                colShape->calculateLocalInertia(mass, localInertia);

                //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
                btMotionState* myMotionState = new btDefaultMotionState(startTransform);
                btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

                auto& object = this->Objects.emplace_back();
                object.first = MakeRef<btRigidBody>(rbInfo);

                auto dir = cameraObject->GetComponent<CameraController>()->GetDirection();
                object.first->setLinearVelocity(btVec2Vec(dir * 180.0f));

                object.second = ShotFactory->MakeInstance();
                object.second->GetComponent<Instance>()->SetColor(Colors::Create(Colors::AQUA));
                object.second->Transform.SetScale(cubeSize);
                this->World->addRigidBody(object.first.get());
            }

            if (InputManager::IsKeyHeld(KeyCode::R))
            {
                Typhoon();
            }

            if (!InputManager::IsMouseHeld(MouseButton::RIGHT))
                this->World->stepSimulation(Time::Delta());

            for (auto& object : this->Objects)
            {
                auto rot = object.first->getWorldTransform().getRotation();
                auto pos = object.first->getWorldTransform().getOrigin();
                object.second->Transform.SetPosition(*(Vector3*)&pos);
                object.second->Transform.SetRotation(*(Quaternion*)&rot);
            }

            if (RuntimeManager::IsEditorActive())
            {
                ImGui::Begin("physics");

                if (ImGui::Button("reset"))
                    this->Reset();
                
                ImGui::InputInt("X", &lenA);
                ImGui::InputInt("Y", &lenB);
                ImGui::InputInt("Z", &lenC);

                ImGui::End();
            }
        }

        void DestroyPhysics()
        {
            for (auto& object : this->Objects)
            {
                this->World->removeRigidBody(object.first.get());
                delete object.first->getMotionState();
                delete object.first->getCollisionShape();
            }
            this->Objects.clear();
        }

        virtual void OnDestroy() override
        {
            this->DestroyPhysics();
            delete bigCube->getMotionState();
            delete bigCube->getCollisionShape();
            this->World->removeRigidBody(bigCube);
            delete bigCube;
        }
    };
}

int main()
{
    ProjectTemplate::MxApplication app;
    app.Run();
    return 0;
}