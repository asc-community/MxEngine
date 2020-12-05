#include <MxEngine.h>

namespace SoundSample
{
    using namespace MxEngine;

    /*
    this sample shows how 3D sounds can be played in the engine
    usually you would like to attach AudioListener to player object (camera)
    then just create another object with AudioSource attached and activate it
    */
    class SoundApplication : public Application
    {
    public:
        virtual void OnCreate() override
        {
            // create camera and global light for a scene
            auto cameraObject = MxObject::Create();
            cameraObject->Name = "Player Camera";

            auto controller = cameraObject->AddComponent<CameraController>();
            controller->ListenWindowResizeEvent();
            Rendering::SetViewport(controller);

            auto input = cameraObject->AddComponent<InputController>();
            input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
            input->BindRotation();


            auto lightObject = MxObject::Create();
            lightObject->Name = "Global Light";
            auto dirLight = lightObject->AddComponent<DirectionalLight>();
            dirLight->SetIntensity(0.5f);
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

        virtual void OnUpdate() override { }

        virtual void OnDestroy() override { }
    };
}

int main()
{
    MxEngine::LaunchFromSourceDirectory();
    SoundSample::SoundApplication app;
    app.Run();
    return 0;
}