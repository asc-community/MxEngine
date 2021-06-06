#include <MxEngine.h>

using namespace MxEngine;

class ShootSpheres : public MxEngine::Scriptable
{
    void ShootSphere(MxObject::Handle sphereFactory, CameraController::Handle playerCamera)
    {    
        auto& player = MxObject::GetByComponent(*playerCamera);
        auto shootPosition = player.LocalTransform.GetPosition() + playerCamera->GetDirection() * 3.0f;
    
        auto sphere = Instanciate(sphereFactory);
        sphere->Name = "Sphere Instance";
        sphere->LocalTransform.SetPosition(shootPosition);
    
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
        if (Input::IsMousePressed(MouseButton::LEFT))
        {
            auto viewport = self.GetComponent<CameraController>();
            if (viewport.IsValid())
            {
                auto sphereFactory = MxObject::GetByName("Sphere Factory");
                if(sphereFactory.IsValid()) this->ShootSphere(sphereFactory, viewport);
            }
        }
    }
};

MXENGINE_RUNTIME_EDITOR(ShootSpheres);
