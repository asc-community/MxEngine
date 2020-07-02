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
            auto cameraObject = MxObject::Create();
            cameraObject->Name = "Player Camera";

            auto controller = cameraObject->AddComponent<CameraController>();
            controller->ListenWindowResizeEvent();
            RenderManager::SetViewport(controller);

            auto input = cameraObject->AddComponent<InputControl>();
            input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
            input->BindRotation();


            auto lightObject = MxObject::Create();
            lightObject->Name = "Global Light";
            auto dirLight = lightObject->AddComponent<DirectionalLight>();
            dirLight->FollowViewport();


            // attach AudioListener component to track movement and rotation of player
            cameraObject->AddComponent<AudioListener>();


            // initialize sound source
            auto soundObject = MxObject::Create();
            soundObject->Name = "Sound Source";
            soundObject->AddComponent<MeshSource>(Primitives::CreateSphere());
            soundObject->AddComponent<MeshRenderer>();

            // add audioSource component
            auto audioSource = soundObject->AddComponent<AudioSource>();
            audioSource->Load(AssetManager::LoadAudio("Resources/never-gonna-give-you-up.mp3"));
            audioSource->Play();

            // custom update behaviour to show that sound is indeed 3D
            struct SoundBehaviour
            {
                float angle  = 0.0f;
                float speed  = 0.4f;
                float radius = 8.0f;

                void OnUpdate(MxObject& object, float dt)
                {
                    angle = std::fmod(angle + speed * dt, TwoPi<float>());
                    float x = radius * std::sin(angle);
                    float z = radius * std::cos(angle);
                    object.Transform.SetPosition(MakeVector3(x, 0.0f, z));
                }
            };
            soundObject->AddComponent<Behaviour>(SoundBehaviour{ });
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