#include <MxEngine.h>

using namespace MxEngine;

class ShootSpheres : public MxEngine::Scriptable
{
    void ShootSphere(InstanceFactory::Handle sphereFactory, CameraController::Handle playerCamera)
    {    
        auto& player = MxObject::GetByComponent(*playerCamera);
        auto shootPosition = player.Transform.GetPosition() + playerCamera->GetDirection() * 3.0f;
    
        auto sphere = sphereFactory->Instanciate();
        sphere->Name = "Sphere Instance";
        sphere->Transform.SetPosition(shootPosition);
    
        sphere->AddComponent<SphereCollider>();
    
        auto body = sphere->AddComponent<RigidBody>();
        body->MakeDynamic();
        body->SetLinearVelocity(playerCamera->GetDirection() * 50.0f);
    
        Timer::CallAfterDelta([sphere]() mutable { MxObject::Destroy(sphere); }, 10.0f);
    }
public:
    virtual void OnCreate(MxEngine::MxObject& self) override
    {
        
    }

    virtual void OnReload(MxEngine::MxObject& self) override
    {
        
    }

    virtual void OnUpdate(MxEngine::MxObject& self) override
    {
        if (Input::IsMouseHeld(MouseButton::LEFT))
        {
            auto viewport = Rendering::GetViewport();
            if (viewport.IsValid())
            {
                auto sphereFactory = self.GetOrAddComponent<InstanceFactory>();
                this->ShootSphere(sphereFactory, viewport);
            }
        }
    }
};

MXENGINE_RUNTIME_EDITOR(ShootSpheres);
