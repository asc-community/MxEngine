#include <MxEngine.h>

namespace PathTracing
{
    using namespace MxEngine;

    class PathTracingApplication : public Application
    {
        ShaderHandle rayTracingShader;
        ShaderHandle postProcessShader;
        Vector3 oldCameraPosition{ 0.0f };
        Vector3 oldCameraDirection{ 0.0f };
        float oldFOV = 0.0f;
        size_t accumulationFrames = 0;
        TextureHandle accumulationTexture;

        virtual void OnCreate() override
        {
            auto camera = MxObject::Create();
            camera->Name = "Camera";
            camera->Transform.SetPosition(Vector3(0.0f, 0.0f, 25.0f));
            auto controller = camera->AddComponent<CameraController>();
            auto input = camera->AddComponent<InputController>();

            Rendering::SetViewport(controller);
            controller->ToggleRendering(false);
            controller->ListensWindowResizeEvent();
            controller->SetMoveSpeed(5.0f);
            controller->SetRotateSpeed(5.0f);
            controller->SetDirection(Vector3(0.0f, 0.0f, -1.0f));

            input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
            input->BindRotation();

            this->rayTracingShader = AssetManager::LoadScreenSpaceShader("path_tracing.glsl"_id);
            this->postProcessShader = AssetManager::LoadScreenSpaceShader("post_process.glsl"_id);
            Runtime::AddShaderUpdateListener(this->rayTracingShader, FileManager::GetWorkingDirectory());
            Runtime::AddShaderUpdateListener(this->postProcessShader, FileManager::GetWorkingDirectory());

            this->accumulationTexture = GraphicFactory::Create<Texture>();
            auto textureSize = Rendering::GetViewportSize();
            
            auto OnResize = [this](WindowResizeEvent& e) mutable
            {
                this->accumulationTexture->Load(nullptr, e.New.x, e.New.y, 3, true, TextureFormat::RGB32F);
                this->accumulationFrames = 0;
            };
            WindowResizeEvent initial(textureSize, textureSize);
            OnResize(initial);
            Event::AddEventListener<WindowResizeEvent>("OnResize", std::move(OnResize));
        }

        virtual void OnUpdate() override
        {
            auto viewport = Rendering::GetViewport();

            auto output = viewport->GetRenderTexture();

            auto viewportSize = Rendering::GetViewportSize();
            auto cameraPosition = MxObject::GetByComponent(*viewport).Transform.GetPosition();
            auto cameraRotation = Vector2{ viewport->GetHorizontalAngle(), viewport->GetVerticalAngle() };
            auto cameraDirection = viewport->GetDirection();
            auto cameraUpVector = viewport->GetDirectionUp();
            auto cameraFOV = viewport->GetCamera<PerspectiveCamera>().GetFOV();

            bool accumulateImage = this->oldCameraPosition == cameraPosition &&
                this->oldCameraDirection == cameraDirection &&
                this->oldFOV == cameraFOV;

            int raySamples = accumulateImage ? 16 : 4;

            this->rayTracingShader->SetUniformInt("uSamples", raySamples);
            this->rayTracingShader->SetUniformFloat("uTime", Time::Current());
            this->rayTracingShader->SetUniformVec2("uViewportSize", viewportSize);
            this->rayTracingShader->SetUniformVec3("uPosition", cameraPosition);
            this->rayTracingShader->SetUniformVec3("uDirection", cameraDirection);
            this->rayTracingShader->SetUniformVec3("uUp", cameraUpVector);
            this->rayTracingShader->SetUniformFloat("uFOV", Radians(cameraFOV));

            if (accumulateImage)
            {
                Rendering::GetController().GetRenderEngine().UseBlending(BlendFactor::ONE, BlendFactor::ONE);
                Rendering::GetController().RenderToTextureNoClear(this->accumulationTexture, this->rayTracingShader);
                accumulationFrames++;
            }
            else
            {
                Rendering::GetController().GetRenderEngine().UseBlending(BlendFactor::ONE, BlendFactor::ZERO);
                Rendering::GetController().RenderToTexture(this->accumulationTexture, this->rayTracingShader);
                accumulationFrames = 1;
            }
            output->GenerateMipmaps();

            this->accumulationTexture->Bind(0);
            this->postProcessShader->SetUniformInt("uImage", this->accumulationTexture->GetBoundId());
            this->postProcessShader->SetUniformInt("uImageSamples", this->accumulationFrames);
            Rendering::GetController().RenderToTexture(output, this->postProcessShader);
            output->GenerateMipmaps();

            this->oldCameraDirection = cameraDirection;
            this->oldCameraPosition = cameraPosition;
            this->oldFOV = cameraFOV;
        }
    };
}

int main()
{
    MxEngine::LaunchFromSourceDirectory();
    PathTracing::PathTracingApplication app;
    app.Run();
    return 0;
}