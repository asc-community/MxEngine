#include <MxEngine.h>

using namespace MxEngine;

class PlazmaGun : public MxEngine::Scriptable
{
    constexpr static Vector3 GunRelativePosition = { -0.05f, -0.06f, 0.025f };
    void FollowPlayer(MxObject& self, const MxObject& player)
    {
        auto camera = player.GetComponent<CameraController>();
        Vector2 rotateAngles = camera->GetRotation();

        self.Transform.SetRotation(Vector3(-rotateAngles.y, rotateAngles.x, 0.0f));
        self.Transform.SetPosition(player.Transform.GetPosition() + self.Transform.GetRotationQuaternion() * GunRelativePosition);
    }
    
    inline static float timeSinceShoot = 0.0f;
    constexpr static float ShootInterval = 0.1f;
    constexpr static float BulletSpeed = 100.0f;

    void Shoot(MxObject& self, const MxObject& player, MxObject& bulletFactory)
    {
        auto camera = player.GetComponent<CameraController>();
        Vector3 viewDirection = camera->GetDirection();

        auto bullet = Instanciate(bulletFactory);

        Vector3 bulletOffset = viewDirection * 0.6f + self.Transform.GetRotationQuaternion() * GunRelativePosition;
        bullet->Transform.SetPosition(player.Transform.GetPosition() + bulletOffset);

        bullet->AddComponent<Script>("PlazmaGunBullet");
        bullet->GetComponent<RigidBody>()->SetLinearVelocity(BulletSpeed * viewDirection);
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
        auto player = MxObject::GetByName("Player");
        auto bulletFactory = MxObject::GetByName("Bullet Factory");

        if (!player.IsValid() || !player->HasComponent<CameraController>())
            return;

        this->FollowPlayer(self, *player);

        if (!bulletFactory.IsValid())
            return;

        timeSinceShoot += Time::Delta();
        if (Input::IsMouseHeld(MouseButton::LEFT) && timeSinceShoot > ShootInterval)
        {
            timeSinceShoot = 0.0f;
            this->Shoot(self, *player, *bulletFactory);
        }
    }
};

MXENGINE_RUNTIME_EDITOR(PlazmaGun);
