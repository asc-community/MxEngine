#pragma once

#include <MxEngine.h>

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
            cameraObject->Name = "Player Camera";
            // add CameraController component which handles camera image rendering
            auto controller = cameraObject->AddComponent<CameraController>();
            // add InpitControl which handles keyboard and mouse input events
            auto input = cameraObject->AddComponent<InputControl>();
            // set camera to change ratio automatically depending on application window size
            controller->ListenWindowResizeEvent();
            // bind player movement to classic WASD mode and space/shift to fly, rotation is done with mouse
            input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
            input->BindRotation();
            // set controller to be main application viewport
            RenderManager::SetViewport(controller);

            // create cube object
            auto cubeObject = MxObject::Create();
            cubeObject->Name = "Cube";
            // move it a bit away from camera
            cubeObject->Transform->Translate(MakeVector3(-1.0f, -1.0f, 3.0f));
            // add mesh to a cube using Primitives class
            auto meshSource = cubeObject->AddComponent<MeshSource>(Primitives::CreateCube());
            // add default (white) material using MeshRenderer component
            auto meshRenderer = cubeObject->AddComponent<MeshRenderer>();

            // create global directional light
            auto lightObject = MxObject::Create();
            lightObject->Name = "Global Light";
            // add DirectionalLight component with custom light direction
            auto dirLight = lightObject->AddComponent<DirectionalLight>();
            dirLight->Direction = MakeVector3(0.5f, 1.0f, 1.0f);
            // make directional light to be centered at current viewport position (is set by RenderManager::SetViewport)
            dirLight->FollowViewport();
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