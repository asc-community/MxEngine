#pragma once

#include <MxEngine.h>

namespace OfflineRendererSample
{
    using namespace MxEngine;

    class OfflineRendererApplication : public Application
    {
        // Configure this parameters //
        VectorInt2 viewportSize{ 7680, 4320 };
        size_t texturesPerRaw = 4;
        float imageSize = 0.2f;
        ///////////////////////////////

        using TextureArray = MxVector<Image>;
        TextureArray textures;

    public:
        virtual void OnCreate() override
        {
            auto cameraObject = MxObject::Create();
            auto controller = cameraObject->AddComponent<CameraController>();
            controller->SetDirection(Vector3(0.0f, -0.333f, 1.0f));
            controller->SetCameraType(CameraType::FRUSTRUM);
            auto skybox = cameraObject->AddComponent<Skybox>();
            skybox->Texture = AssetManager::LoadCubeMap("Resources/dawn.jpg");

            Rendering::SetViewport(controller);
            Rendering::ResizeViewport(viewportSize.x, viewportSize.y);

            auto cubeObject = MxObject::Create();
            cubeObject->Transform.Translate(MakeVector3(0.0f, -1.0f, 3.0f));
            cubeObject->Transform.RotateY(45.0f);

            auto meshSource = cubeObject->AddComponent<MeshSource>(Primitives::CreateCube());
            auto meshRenderer = cubeObject->AddComponent<MeshRenderer>();
            auto yellowColor = MakeVector3(1.0f, 0.7f, 0.0f);
            meshRenderer->GetMaterial()->DiffuseColor = yellowColor;
            meshRenderer->GetMaterial()->AmbientColor = yellowColor;
            meshRenderer->GetMaterial()->SpecularColor = yellowColor;

            auto lightObject = MxObject::Create();
            auto dirLight = lightObject->AddComponent<DirectionalLight>();
            dirLight->Direction = MakeVector3(0.5f, 1.0f, -0.1f);
            dirLight->FollowViewport();
        }

        virtual void OnUpdate() override
        {
            static int frame_count = 0;
            auto& viewport = *Rendering::GetViewport();

            auto& cam = viewport.GetCamera<FrustrumCamera>();
            cam.SetProjectionForTile(frame_count % texturesPerRaw, frame_count / texturesPerRaw, texturesPerRaw, imageSize);

            if (frame_count != 0)
            {
                auto texture = Rendering::GetViewport()->GetRenderTexture();
                textures.push_back(texture->GetRawTextureData());
            }
            if (frame_count == texturesPerRaw * texturesPerRaw)
            {
                auto result = ImageManager::CombineImages(textures, texturesPerRaw);
                auto png = ImageConverter::ConvertImagePNG(result);
                File file("Resources/scene.png", File::WRITE | File::BINARY);
                file.WriteBytes(png.data(), png.size());
                this->CloseApplication();
            }
            frame_count++;
        }

        virtual void OnDestroy() override
        {

        }
    };
}

int main()
{
    OfflineRendererSample::OfflineRendererApplication app;
    app.Run();
    return 0;
}