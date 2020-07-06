#pragma once

#include <MxEngine.h>

#include <Vendors/bullet3/btBulletDynamicsCommon.h>

namespace ProjectTemplate
{
    using namespace MxEngine;

    class MxApplication : public Application
    {
    public:
        virtual void OnCreate() override
        {
            auto cameraObject = MxObject::Create();
            cameraObject->Name = "Player Camera";
            auto controller = cameraObject->AddComponent<CameraController>();
            auto input = cameraObject->AddComponent<InputControl>();
            controller->ListenWindowResizeEvent();
            input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
            input->BindRotation();
            RenderManager::SetViewport(controller);

            auto cubeObject = MxObject::Create();
            cubeObject->Name = "Cube";
            cubeObject->Transform.Translate(MakeVector3(-1.0f, -1.0f, 3.0f));
            auto meshSource = cubeObject->AddComponent<MeshSource>(Primitives::CreateCube());
            auto meshRenderer = cubeObject->AddComponent<MeshRenderer>();

            // create global directional light
            auto lightObject = MxObject::Create();
            lightObject->Name = "Global Light";
            auto dirLight = lightObject->AddComponent<DirectionalLight>();
            dirLight->Direction = MakeVector3(0.5f, 1.0f, 1.0f);
            dirLight->FollowViewport();

            // TODO: make this work :)

            btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

            ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
            btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

            ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
            btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

            ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
            btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

            btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

            dynamicsWorld->setGravity(btVector3(0.0f, -9.8f, 0.0f));
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