#pragma once

#include <MxEngine.h>
#include <Library/Primitives/Colors.h>

namespace ProjectTemplate
{
    using namespace MxEngine;

    class MxApplication : public Application
    {
    public:
        virtual void OnCreate() override
        {
            auto cameraObject = MxObject::Create();
            auto controller = cameraObject->AddComponent<CameraController>();
            controller->SetDirection(Vector3(0.0f, -0.333f, 1.0f));

            // set viewport to 8K
            RenderManager::SetViewport(controller);
            RenderManager::ResizeViewport(7680, 4320);

            auto cubeObject = MxObject::Create();
            cubeObject->Transform->Translate(MakeVector3(0.0f, -1.0f, 3.0f));
            cubeObject->Transform->RotateY(45.0f);

            auto meshSource = cubeObject->AddComponent<MeshSource>(Primitives::CreateCube());
            auto meshRenderer = cubeObject->AddComponent<MeshRenderer>();
            auto yellowColor = MakeVector3(1.0f, 0.7f, 0.0f);
            meshRenderer->GetMaterial()->DiffuseColor  = yellowColor;
            meshRenderer->GetMaterial()->AmbientColor  = yellowColor;
            meshRenderer->GetMaterial()->SpecularColor = yellowColor;

            auto lightObject = MxObject::Create();
            auto dirLight = lightObject->AddComponent<DirectionalLight>();
            dirLight->Direction = MakeVector3(0.5f, 1.0f, -0.1f);
            dirLight->FollowViewport();
        }

        virtual void OnUpdate() override
        {
            static bool firstFrame = true;
            if (firstFrame)
            {
                firstFrame = false;
            }
            else
            {
                ImageManager::TakeScreenShot("Resources/cube.png", ImageType::PNG);
                this->CloseApplication();
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