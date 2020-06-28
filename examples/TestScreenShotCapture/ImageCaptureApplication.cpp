#pragma once

#include <MxEngine.h>
#include <Utilities/Image/ImageConverter.h>

namespace ProjectTemplate
{
    using namespace MxEngine;

    class MxApplication : public Application
    {
        // Configure this parameters //
        VectorInt2 viewportSize{ 640, 360 };
        size_t texturesPerRaw = 4;
        float offset = 0.05f;
        ///////////////////////////////

        using TextureArray = MxVector<MxVector<uint8_t>>;
        TextureArray textures;

        static MxVector<uint8_t> CombineTextures(const TextureArray& textures, size_t width, size_t height, size_t channels)
        {
            MxVector<uint8_t> result;
            result.reserve(width * height * channels * textures.size());

            // write order:
            // 3 4
            // 1 2
            
            // 7 8 9
            // 4 5 6
            // 1 2 3

            size_t texPerRow = (size_t)std::sqrt(textures.size());

            for (size_t t1 = 0; t1 < texPerRow; t1++)
            {
                for (size_t i = 0; i < height; i++)
                {
                    for (size_t t2 = 0; t2 < texPerRow; t2++)
                    {
                        auto& tex = textures[t1 * texPerRow + t2];
                        result.insert(result.end(), tex.begin() + i * width * channels, tex.begin() + (i + 1) * width * channels);
                    }
                }
            }
            return result;
        }

        static Matrix4x4 GetProjectionMatrixForTileN(size_t tileIndex, size_t tilesPerRaw, 
            float cameraAspect, float znear, float zfar, float offset)
        {
            float initialOffsetX = -0.5f * tilesPerRaw * offset * cameraAspect;
            float initialOffsetY = -0.5f * tilesPerRaw * offset;
            float x = initialOffsetX + float(tileIndex % tilesPerRaw) * offset * cameraAspect;
            float y = initialOffsetY + float(tileIndex / tilesPerRaw) * offset;
            return MakeFrustrumMatrix(x, x + offset * cameraAspect, y, y + offset, znear, zfar);
        }

    public:
        virtual void OnCreate() override
        {
            auto cameraObject = MxObject::Create();
            auto controller = cameraObject->AddComponent<CameraController>();
            controller->SetDirection(Vector3(0.0f, -0.333f, 1.0f));
            controller->Camera.SetZFar(1000000.0f);
            controller->SetCameraType(CameraType::ORTHOGRAPHIC);
            auto skybox = cameraObject->AddComponent<Skybox>();
            skybox->Texture = AssetManager::LoadCubeMap("Resources/dawn.jpg");

            RenderManager::SetViewport(controller);
            RenderManager::ResizeViewport(viewportSize.x, viewportSize.y);

            auto cubeObject = MxObject::Create();
            cubeObject->Transform->Translate(MakeVector3(0.0f, -1.0f, 3.0f));
            cubeObject->Transform->RotateY(45.0f);

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
            auto& viewport = *RenderManager::GetViewport();
            
            float aspect = float(viewportSize.x) / float(viewportSize.y);
            float zfar = viewport.Camera.GetZFar();
            float znear = viewport.Camera.GetZNear();

            auto Projection = GetProjectionMatrixForTileN(frame_count, texturesPerRaw, aspect, znear, zfar, offset);
            viewport.Camera.SetProjectionMatrix(Projection);

            if (frame_count != 0)
            {
                auto texture = RenderManager::GetViewport()->GetRenderTexture();
                textures.push_back(texture->GetRawTextureData());
            }
            if (frame_count == texturesPerRaw * texturesPerRaw)
            {
                auto result = CombineTextures(textures, viewportSize.x, viewportSize.y, 3);
                auto png = ImageConverter::ConvertImagePNG(result.data(), viewportSize.x * texturesPerRaw, viewportSize.y * texturesPerRaw, 3);
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
    ProjectTemplate::MxApplication app;
    app.Run();
    return 0;
}