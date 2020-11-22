#include <MxEngine.h>

namespace RayTracing
{
    using namespace MxEngine;

    class RayTracingApplication : public Application
    {
        ShaderHandle rayTracingShader;

        virtual void OnCreate() override
        {
            auto camera = MxObject::Create();
            auto controller = camera->AddComponent<CameraController>();
            auto input = camera->AddComponent<InputController>();
            auto skybox = camera->AddComponent<Skybox>();
            skybox->CubeMap = AssetManager::LoadCubeMap("skybox.png"_id);

            Rendering::SetViewport(controller);
            controller->ToggleRendering(false);
            controller->ListensWindowResizeEvent();

            input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
            input->BindRotation();

            this->rayTracingShader = AssetManager::LoadScreenSpaceShader("ray_tracing.glsl"_id);
            Runtime::AddShaderUpdateListener(this->rayTracingShader, FileManager::GetWorkingDirectory());
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
            auto skybox = MxObject::GetByComponent(*viewport).GetComponent<Skybox>()->CubeMap;

            skybox->Bind(0);

            this->rayTracingShader->SetUniformFloat("uTime", Time::Current());
            this->rayTracingShader->SetUniformVec2("uViewportSize", viewportSize);
            this->rayTracingShader->SetUniformVec3("uPosition", cameraPosition);
            this->rayTracingShader->SetUniformVec3("uDirection", cameraDirection);
            this->rayTracingShader->SetUniformVec3("uUp", cameraUpVector);
            this->rayTracingShader->SetUniformFloat("uFOV", Radians(cameraFOV));
            this->rayTracingShader->SetUniformInt("uEnvironment", skybox->GetBoundId());

            Rendering::GetController().RenderToTexture(output, this->rayTracingShader);
            output->GenerateMipmaps();
        }
    };
}

int main()
{
    RayTracing::RayTracingApplication app;
    app.Run();
    return 0;
}