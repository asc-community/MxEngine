#include <MxEngine.h>

namespace OfflineRendererSample
{
    using namespace MxEngine;

    /*
    this sample shows how to render one big screenshot of a scene and save it to disk
    although it is possible to just resize camera render texture,  
    such image will be bound by gpu memory. To avoid this, here we render image in multiple frames
    tile-by-tile using frustrum camera projection. Resulting image size is (viewportSize * texturesPerRaw)
    */
    class OfflineRendererApplication : public Application
    {
        // Configure this parameters as you wish //
        VectorInt2 viewportSize{ 7680, 4320 };
        size_t texturesPerRow = 4;
        float imageSize = 0.2f;
        ///////////////////////////////////////////

        using TextureArray = MxVector<Image>;
        TextureArray textures;

    public:
        virtual void OnCreate() override
        {
            // create camera with frustrum mode enabled
            auto cameraObject = MxObject::Create();
            auto controller = cameraObject->AddComponent<CameraController>();
            controller->SetDirection(Vector3(0.0f, -0.333f, 1.0f));
            controller->SetCameraType(CameraType::FRUSTRUM);
            auto effects = cameraObject->AddComponent<CameraEffects>();
            effects->SetVignetteRadius(0.0f);
            auto skybox = cameraObject->AddComponent<Skybox>();
            skybox->CubeMap = AssetManager::LoadCubeMap("Resources/dawn.jpg"_id);
            skybox->Irradiance = AssetManager::LoadCubeMap("Resources/dawn_irradiance.jpg"_id);

            Rendering::SetViewport(controller);
            Rendering::ResizeViewport(viewportSize.x, viewportSize.y);

            // create yellow cube as main rendering target
            auto cubeObject = MxObject::Create();
            cubeObject->Transform.Translate(MakeVector3(0.0f, -1.0f, 3.0f));
            cubeObject->Transform.RotateY(45.0f);

            auto meshSource = cubeObject->AddComponent<MeshSource>(Primitives::CreateCube());
            auto meshRenderer = cubeObject->AddComponent<MeshRenderer>();
            auto yellowColor = MakeVector3(1.0f, 0.7f, 0.0f);
            meshRenderer->GetMaterial()->BaseColor = yellowColor;

            // create global light
            auto lightObject = MxObject::Create();
            auto dirLight = lightObject->AddComponent<DirectionalLight>();
            dirLight->Direction = MakeVector3(0.5f, 1.0f, -0.1f);
            dirLight->FollowViewport();
        }

        virtual void OnUpdate() override
        {
            static int frameCount = 0;

            // we determine current tile by frames passed. Total number of frames should be texturesPerRow^2
            auto& cam = Rendering::GetViewport()->GetCamera<FrustrumCamera>();
            cam.SetProjectionForTile(frameCount % texturesPerRow, frameCount / texturesPerRow, texturesPerRow, imageSize);

            if (frameCount != 0) // avoid submitting empty texture, as on zero frame there is no image rendered
            {
                auto texture = Rendering::GetViewport()->GetRenderTexture();
                textures.push_back(texture->GetRawTextureData());
            }
            if (frameCount == texturesPerRow * texturesPerRow) // when we reach last frame, get image data and convert it to png format
            {
                auto result = ImageManager::CombineImages(textures, texturesPerRow);
                auto png = ImageConverter::ConvertImagePNG(result);
                File file("Resources/scene.png", File::WRITE | File::BINARY);
                file.WriteBytes(png.data(), png.size());
                this->CloseApplication();
            }
            frameCount++;
        }

        virtual void OnDestroy() override { }
    };
}

int main()
{
    MxEngine::LaunchFromSourceDirectory();
    OfflineRendererSample::OfflineRendererApplication app;
    app.Run();
    return 0;
}