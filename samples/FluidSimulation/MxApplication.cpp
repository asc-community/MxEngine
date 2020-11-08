#include <MxEngine.h>

namespace FluidSimulation
{
    using namespace MxEngine;

    class FluidSimulationApplication : public Application
    {
        ShaderHandle vorticityComputeShader;
        ShaderHandle vorticityApplyShader;
        ShaderHandle diffuseApplyShader;

        TextureHandle vorticityField;
        TextureHandle particleField;
        TextureHandle particleSwapField;

        virtual void OnCreate() override
        {
            auto camera = MxObject::Create();
            auto controller = camera->AddComponent<CameraController>();
            Rendering::SetViewport(controller);

            Vector<2, int> textureSize = GlobalConfig::GetWindowSize();

            controller->ToggleRendering(false);
            controller->SetRenderTexture(Colors::MakeTexture(Colors::RED));

            vorticityComputeShader = AssetManager::LoadScreenSpaceShader("vorticity_compute.glsl"_id);
            vorticityComputeShader = AssetManager::LoadScreenSpaceShader("vorticity_apply.glsl"_id);
            diffuseApplyShader = AssetManager::LoadScreenSpaceShader("diffuse_apply.glsl"_id);

            vorticityField = GraphicFactory::Create<Texture>();
            vorticityField->Load(nullptr, textureSize.x, textureSize.y, 4, false, TextureFormat::RGBA32F);

            particleField = GraphicFactory::Create<Texture>();
            particleField->Load(nullptr, textureSize.x, textureSize.y, 4, false, TextureFormat::RGBA32F);

            particleSwapField = GraphicFactory::Create<Texture>();
            particleSwapField->Load(nullptr, textureSize.x, textureSize.y, 4, false, TextureFormat::RGBA32F);
        }

        virtual void OnUpdate() override
        {
            float vorticityScale = 50.0f;
            float diffusionScale = 0.8f;;
            size_t diffuseIterations = 20;

            auto output = Rendering::GetViewport()->GetRenderTexture();

            vorticityComputeShader->SetUniformInt("particleTex", 0);
            particleField->Bind(0);

            Rendering::GetController().RenderToTexture(vorticityField, vorticityComputeShader);

            vorticityApplyShader->SetUniformInt("particleTex", 0);
            particleField->Bind(0);
            vorticityApplyShader->SetUniformInt("vorticityTex", 1);
            vorticityField->Bind(1);

            vorticityApplyShader->SetUniformFloat("timeDelta", Time::Delta());
            vorticityApplyShader->SetUniformFloat("vorticityScale", vorticityScale);

            Rendering::GetController().RenderToTexture(particleSwapField, vorticityApplyShader);
            std::swap(particleField, particleSwapField);

            for (size_t i = 0; i < diffuseIterations; i++)
            {
                diffuseApplyShader->SetUniformFloat("timeDelta", Time::Delta());
                diffuseApplyShader->SetUniformFloat("diffusionScale", diffusionScale);
                std::swap(particleField, particleSwapField);
            }
        }
    };
}

int main()
{
    FluidSimulation::FluidSimulationApplication app;
    app.Run();
    return 0;
}