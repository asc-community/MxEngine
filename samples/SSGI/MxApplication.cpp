#include <MxEngine.h>

namespace SSGI
{
    using namespace MxEngine;

    struct Particle
    {
        Vector2 position;
    };

    class SSGIApplication : public Application
    {
        CameraControllerHandle Camera;
        ShaderStorageBufferHandle ParticlePositionSSBO;
        ShaderStorageBufferHandle ParticleVelocitySSBO;
        ComputeShaderHandle ParticleCompute;
        ShaderHandle ParticleApply;

        virtual void OnCreate() override
        {
            auto camera = MxObject::Create();
            camera->Name = "Camera";

            this->Camera = camera->AddComponent<CameraController>();
            this->Camera->SetMoveSpeed(10.0f);
            this->Camera->ToggleRendering(false);
            Rendering::SetViewport(this->Camera);

            camera->Transform.TranslateY(15.0f);
            
            auto skybox = camera->AddComponent<Skybox>();
            skybox->CubeMap = AssetManager::LoadCubeMap("Resources/skybox.png"_id);
            skybox->Irradiance = AssetManager::LoadCubeMap("Resources/skybox_irradiance.png"_id);
            skybox->SetIntensity(1.0);
            
            auto input = camera->AddComponent<InputController>();
            input->BindMovementWASDSpaceShift();
            input->BindRotation();

            auto toneMapping = camera->AddComponent<CameraToneMapping>();
            toneMapping->SetMinLuminance(0.3f);
            toneMapping->SetWhitePoint(0.5f);

            auto effects = camera->AddComponent<CameraEffects>();
            effects->SetFogDensity(0.0f);

            auto ssgi = camera->AddComponent<CameraSSGI>();

            // auto sponza = MxObject::Create();
            // sponza->Name = "Sponza";
            // sponza->AddComponent<MeshSource>(AssetManager::LoadMesh("Resources/Sponza/glTF/Sponza.gltf"_id));
            // sponza->AddComponent<MeshRenderer>(AssetManager::LoadMaterials("Resources/Sponza/glTF/Sponza.gltf"_id));
            // sponza->Transform.SetScale(0.02f);
            // sponza->Transform.TranslateY(13.0f);

            #define PARTICLE_COUNT (256 * 32 * 32)

            this->ParticlePositionSSBO = GraphicFactory::Create<ShaderStorageBuffer>((Particle*)nullptr, PARTICLE_COUNT, UsageType::DYNAMIC_DRAW);
            this->ParticleVelocitySSBO = GraphicFactory::Create<ShaderStorageBuffer>((Particle*)nullptr, PARTICLE_COUNT, UsageType::DYNAMIC_DRAW);
            this->ParticleCompute = AssetManager::LoadComputeShader("Resources/particle_compute.glsl"_id);
            this->ParticleApply = AssetManager::LoadScreenSpaceShader("Resources/particle_apply_fragment.glsl"_id);

            this->ParticleCompute->Bind();
            this->ParticlePositionSSBO->BindBase(0);
            this->ParticleVelocitySSBO->BindBase(1);

            this->ParticleApply->Bind();
            this->ParticlePositionSSBO->BindBase(0);

            Runtime::AddShaderUpdateListener(this->ParticleCompute);
            Runtime::AddShaderUpdateListener(this->ParticleApply);
        }

        virtual void OnUpdate() override
        {
            this->ParticleCompute->Bind();
            this->ParticleCompute->SetUniform("dt", Time::Delta());
            Compute::Dispatch(this->ParticleCompute, 32 * 32, 1, 1);

            auto output = this->Camera->GetRenderTexture();

            Compute::SetMemoryBarrier(BarrierType::SHADER_STORAGE_BUFFER);
            Rendering::GetController().RenderToTexture(output, this->ParticleApply);
        }
    };
}

int main()
{
    MxEngine::LaunchFromSourceDirectory();
    SSGI::SSGIApplication app;
    app.Run();
    return 0;
}