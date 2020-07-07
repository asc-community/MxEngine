#pragma once

#include <MxEngine.h>

#include <Vendors/bullet3/btBulletDynamicsCommon.h>

namespace ProjectTemplate
{
    using namespace MxEngine;

    class MxApplication : public Application
    {
    public:
        UniqueRef<btCollisionConfiguration> CollisionConfiguration;
        UniqueRef<btDispatcher> Dispatcher;
        UniqueRef<btBroadphaseInterface>  Broadphase;
        UniqueRef<btConstraintSolver> Solver;
        UniqueRef<btDiscreteDynamicsWorld> World;
        MxVector<std::pair<UniqueRef<btRigidBody>, MxObject::Handle>> Objects;

        virtual void OnCreate() override
        {
            auto cameraObject = MxObject::Create();
            cameraObject->Name = "Player Camera";
            auto controller = cameraObject->AddComponent<CameraController>();
            auto input = cameraObject->AddComponent<InputControl>();
            controller->ListenWindowResizeEvent();
            controller->SetMoveSpeed(10.0f);
            input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
            input->BindRotation();
            RenderManager::SetViewport(controller);

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

            {
                btCollisionShape* playerShape = new btSphereShape(1.0f);
                
                btTransform playerTransform;
                playerTransform.setIdentity();

                btScalar mass(50.0f);
                btVector3 localInertia(0.0f, 0.0f, 0.0f);
                playerShape->calculateLocalInertia(mass, localInertia);

                btMotionState* myMotionState = new btDefaultMotionState(playerTransform);
                btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, playerShape, localInertia);

                auto& object = this->Objects.emplace_back();
                object.first = MakeUnique<btRigidBody>(rbInfo);
                object.second = cameraObject;

                this->World->addRigidBody(object.first.get());
            }

            {

                float size = 50.0f;

                btCollisionShape* groundShape = new btBoxShape(btVector3(0.5f * size, 0.5f * size, 0.5f * size));               

                btTransform groundTransform;
                groundTransform.setIdentity();
                groundTransform.setOrigin(btVector3(0, -size * 0.5f - 6.0f, 0));

                btScalar mass(0.0f);
                btVector3 localInertia(0, 0, 0);

                //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
                btMotionState* myMotionState = new btDefaultMotionState(groundTransform);
                btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);

                //add the body to the dynamics world
                auto& object = this->Objects.emplace_back();
                object.first = MakeUnique<btRigidBody>(rbInfo);
                object.second = MxObject::Create();
                object.second->Name = "Big Cube";
                object.second->AddComponent<MeshRenderer>();
                object.second->AddComponent<MeshSource>(Primitives::CreateCube());
                object.second->Transform.SetScale(size);

                this->World->addRigidBody(object.first.get());
            }

            for (size_t i = 0; i < 30; i++)
            {
                float cubeSize = 0.5f;

                btCollisionShape* colShape = new btBoxShape(btVector3(cubeSize * 0.5f, cubeSize * 0.5f, cubeSize * 0.5f));

                float x = 5.0f * Random::GetFloat() - 2.5f;
                float y = 100.0f;
                float z = 5.0f * Random::GetFloat() - 2.5f;

                btTransform startTransform;
                startTransform.setIdentity();
                startTransform.setOrigin(btVector3(x, y, z));

                btScalar mass(1.f);
                btVector3 localInertia(0, 0, 0);
                colShape->calculateLocalInertia(mass, localInertia);

                //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
                btMotionState* myMotionState = new btDefaultMotionState(startTransform);
                btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

                auto& object = this->Objects.emplace_back();
                object.first = MakeUnique<btRigidBody>(rbInfo);
                object.second = MxObject::Create();
                object.second->Name = MxFormat("Sphere #{0}", i);
                object.second->AddComponent<MeshRenderer>()->GetMaterial()->BaseColor = Colors::Create(Colors::RED);
                object.second->AddComponent<MeshSource>(Primitives::CreateCube());
                object.second->Transform.SetScale(cubeSize);

                this->World->addRigidBody(object.first.get());
            }
        }

        virtual void OnUpdate() override
        {

            auto& player = this->Objects.front();
            auto pos = *(btVector3*)&player.second->Transform.GetPosition();
            btTransform playerTransform;

            playerTransform.setIdentity();
            playerTransform.setOrigin(pos);
            player.first->setWorldTransform(playerTransform);

            this->World->stepSimulation(Time::Delta());

            for (auto& object : this->Objects)
            {
                auto rot = object.first->getWorldTransform().getRotation();
                auto pos = object.first->getWorldTransform().getOrigin();
                object.second->Transform.SetPosition(*(Vector3*)&pos);
                object.second->Transform.SetRotation(*(Quaternion*)&rot);
            }
        }

        virtual void OnDestroy() override
        {
            for (auto& object : this->Objects)
            {
                this->World->removeRigidBody(object.first.get());
                delete object.first->getMotionState();
                delete object.first->getCollisionShape();
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