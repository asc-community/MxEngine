#include <MxEngine.h>

namespace Sponza
{
    using namespace MxEngine;

    class SponzaApplication : public Application
    {
        MxObject::Handle sphereFactory;

        void ShootSphere()
        {
            auto controller = Rendering::GetViewport();
            auto& player = MxObject::GetByComponent(*Rendering::GetViewport());
            
            auto shootPosition = player.Transform.GetPosition() + controller->GetDirection() * 3.0f;

            auto sphere = this->sphereFactory->GetComponent<InstanceFactory>()->MakeInstance();
            sphere->Name = "Sphere Instance";
            sphere->IsSerialized = false;
            sphere->Transform.SetPosition(shootPosition);

            sphere->AddComponent<SphereCollider>();

            auto body = sphere->AddComponent<RigidBody>();
            body->MakeDynamic();
            body->SetLinearVelocity(controller->GetDirection() * 50.0f);

            Timer::CallAfterDelta([sphere]() mutable { MxObject::Destroy(sphere); }, 10.0f);
        }

        virtual void OnCreate() override
        {
            auto camera = MxObject::Create();
            camera->Name = "Camera";

            auto controller = camera->AddComponent<CameraController>();
            controller->SetMoveSpeed(10.0f);

            camera->Transform.TranslateY(15.0f);
            
            auto skybox = camera->AddComponent<Skybox>();
            skybox->CubeMap = AssetManager::LoadCubeMap("Resources/skybox.png"_id);
            skybox->Irradiance = AssetManager::LoadCubeMap("Resources/skybox_irradiance.png"_id);
            skybox->SetIntensity(1.0);
            
            auto input = camera->AddComponent<InputController>();
            input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
            input->BindRotation();

            auto toneMapping = camera->AddComponent<CameraToneMapping>();
            toneMapping->SetMinLuminance(0.3f);
            toneMapping->SetWhitePoint(0.5f);

            auto ssr = camera->AddComponent<CameraSSR>();

            auto effects = camera->AddComponent<CameraEffects>();
            effects->SetFogDensity(0.0f);

            camera->AddComponent<CapsuleCollider>()->SetBoundingCapsule(Capsule(4.5f, 0.75f, Capsule::Axis::Y));
            camera->AddComponent<RigidBody>();
            auto character = camera->AddComponent<CharacterController>();
            character->SetJumpSpeed(10.0f);
            character->SetJumpPower(12.0f);

            Rendering::SetViewport(controller);

            auto lightObject = MxObject::Create();
            lightObject->Name = "Global Light";
            auto dirLight = lightObject->AddComponent<DirectionalLight>();
            dirLight->Projections[1] = 100.0f;
            dirLight->SetIntensity(5.0f);
            dirLight->SetAmbientIntensity(0.25f);
            dirLight->FollowViewport();

            this->sphereFactory = MxObject::Create();
            this->sphereFactory->Name = "Sphere Factory";
            this->sphereFactory->AddComponent<InstanceFactory>();
            this->sphereFactory->AddComponent<MeshSource>(Primitives::CreateSphere(20));
            auto renderer = this->sphereFactory->AddComponent<MeshRenderer>();
            renderer->GetMaterial()->AlbedoMap = AssetManager::LoadTexture("Resources/moon.jpg"_id);

            auto sponza = MxObject::Create();
            sponza->Name = "Sponza";
            sponza->AddComponent<MeshSource>(AssetManager::LoadMesh("Resources/Sponza/glTF/Sponza.gltf"_id));
            sponza->AddComponent<MeshRenderer>(AssetManager::LoadMaterials("Resources/Sponza/glTF/Sponza.gltf"_id));
            sponza->Transform.SetScale(0.02f);
            sponza->Transform.TranslateY(13.0f);
            
            auto& materials = sponza->GetComponent<MeshRenderer>()->Materials;
            if (!materials.empty())
            {
                // floor material
                materials[8]->Name = "Floor";
                materials[8]->MetallicFactor = 0.25f;
                materials[8]->RoughnessFactor = 0.25f;
                materials[8]->MetallicMap = { };

                // lion materials
                materials[3]->Name = "Lion";
                materials[3]->MetallicFactor = 1.0f;
                materials[3]->RoughnessFactor = 0.0f;

                materials[23]->Name = "Lion Head";
                materials[23]->MetallicFactor = 1.0f;
                materials[23]->RoughnessFactor = 0.0f;
            }

            sponza->AddComponent<RigidBody>();
            auto collider = sponza->AddComponent<CompoundCollider>();
            // ground
            collider->AddShape<BoxShape>(
                TransformComponent{ }.Translate(Vector3(0.0f, -725.0f, 0.0f)),
                BoundingBox(MakeVector3(0.0f), MakeVector3(1800.0f, 75.0f, 1075.0f))
                );
            // long wall
            collider->AddShape<BoxShape>(
                TransformComponent{ }.Translate(Vector3(0.0f, 0.0f, 850.0f)),
                BoundingBox(MakeVector3(0.0f), MakeVector3(1800.0f, 750.0f, 250.0f))
                );
            // long wall
            collider->AddShape<BoxShape>(
                TransformComponent{ }.Translate(Vector3(0.0f, 0.0f, -850.0f)),
                BoundingBox(MakeVector3(0.0f), MakeVector3(1800.0f, 750.0f, 250.0f))
                );
            // short wall
            collider->AddShape<BoxShape>(
                TransformComponent{ }.Translate(Vector3(1600.0f, 0.0f, 0.0f)),
                BoundingBox(MakeVector3(0.0f), MakeVector3(250.0f, 750.0f, 1075.0f))
                );
            // short wall
            collider->AddShape<BoxShape>(
                TransformComponent{ }.Translate(Vector3(-1600.0f, 0.0f, 0.0f)),
                BoundingBox(MakeVector3(0.0f), MakeVector3(250.0f, 750.0f, 1075.0f))
                );
            // columns
            collider->AddShape<CylinderShape>(
                TransformComponent{ }.Translate(Vector3(1180.0f, -600.0f, 440.0f)),
                Cylinder(100.0f, 75.0f, 75.0f, Cylinder::Axis::Y)
                );
            collider->AddShape<CylinderShape>(
                TransformComponent{ }.Translate(Vector3(1180.0f, -600.0f, -410.0f)),
                Cylinder(100.0f, 75.0f, 75.0f, Cylinder::Axis::Y)
                );
            collider->AddShape<CylinderShape>(
                TransformComponent{ }.Translate(Vector3(-1140.0f, -600.0f, 440.0f)),
                Cylinder(100.0f, 75.0f, 75.0f, Cylinder::Axis::Y)
                );
            collider->AddShape<CylinderShape>(
                TransformComponent{ }.Translate(Vector3(-1140.0f, -600.0f, -410.0f)),
                Cylinder(100.0f, 75.0f, 75.0f, Cylinder::Axis::Y)
                );
            // inner wall
            collider->AddShape<BoxShape>(
                TransformComponent{ }.Translate(Vector3(0.0f, -425.0f, -225.0f)),
                BoundingBox(MakeVector3(0.0f), MakeVector3(980.0f, 300.0f, 40.0f))
                );
            // inner wall
            collider->AddShape<BoxShape>(
                TransformComponent{ }.Translate(Vector3(0.0f, -425.0f, 225.0f)),
                BoundingBox(MakeVector3(0.0f), MakeVector3(980.0f, 300.0f, 40.0f))
                );
            // balks
            collider->AddShape<BoxShape>(
                TransformComponent{ }.Translate(Vector3(950.0f, -175.0f, 0.0f)),
                BoundingBox(MakeVector3(0.0f), MakeVector3(40.0f, 50.0f, 265.0f))
                );
            collider->AddShape<BoxShape>(
                TransformComponent{ }.Translate(Vector3(-950.0f, -175.0f, 0.0f)),
                BoundingBox(MakeVector3(0.0f), MakeVector3(40.0f, 50.0f, 265.0f))
                );
            // second floor
            collider->AddShape<BoxShape>(
                TransformComponent{ }.Translate(Vector3(0.0f, -235.0f, 450.0f)),
                BoundingBox(MakeVector3(0.0f), MakeVector3(1800.0f, 10.0f, 250.0f))
                );
            collider->AddShape<BoxShape>(
                TransformComponent{ }.Translate(Vector3(0.0f, -235.0f, -450.0f)),
                BoundingBox(MakeVector3(0.0f), MakeVector3(1800.0f, 10.0f, 250.0f))
                );
            collider->AddShape<BoxShape>(
                TransformComponent{ }.Translate(Vector3(1175.0f, -235.0f, 0.0f)),
                BoundingBox(MakeVector3(0.0f), MakeVector3(250.0f, 10.0f, 1075.0f))
                );
            collider->AddShape<BoxShape>(
                TransformComponent{ }.Translate(Vector3(-1175.0f, -235.0f, 0.0f)),
                BoundingBox(MakeVector3(0.0f), MakeVector3(250.0f, 10.0f, 1075.0f))
                );
            // ceiling
            collider->AddShape<BoxShape>(
                TransformComponent{ }.Translate(Vector3(0.0f, 500.0f, 450.0f)),
                BoundingBox(MakeVector3(0.0f), MakeVector3(1800.0f, 250.0f, 250.0f))
                );
            collider->AddShape<BoxShape>(
                TransformComponent{ }.Translate(Vector3(0.0f, 500.0f, -450.0f)),
                BoundingBox(MakeVector3(0.0f), MakeVector3(1800.0f, 250.0f, 250.0f))
                );
            collider->AddShape<BoxShape>(
                TransformComponent{ }.Translate(Vector3(1175.0f, 500.0f, 0.0f)),
                BoundingBox(MakeVector3(0.0f), MakeVector3(250.0f, 250.0f, 1075.0f))
                );
            collider->AddShape<BoxShape>(
                TransformComponent{ }.Translate(Vector3(-1175.0f, 500.0f, 0.0f)),
                BoundingBox(MakeVector3(0.0f), MakeVector3(250.0f, 250.0f, 1075.0f))
                );
        }

        virtual void OnUpdate() override
        {
            static float timeSinceShoot = 0.0f;
            timeSinceShoot += Time::Delta();

            if (timeSinceShoot > 0.3f && Input::IsMouseHeld(MouseButton::LEFT))
            {
                this->ShootSphere();
            }
            if (Input::IsKeyPressed(KeyCode::ESCAPE))
                this->CloseApplication();
        }
    };
}

int main()
{
    MxEngine::LaunchFromSourceDirectory();
    Sponza::SponzaApplication app;
    app.Run();
    return 0;
}