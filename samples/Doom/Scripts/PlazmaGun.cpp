#include <MxEngine.h>

using namespace MxEngine;

class PlazmaGun : public MxEngine::Scriptable
{
    constexpr static Vector3 GunRelativePosition = { -0.05f, -0.06f, 0.025f };

    void FollowPlayer(MxObject::Handle self, MxObject::Handle player)
    {
        auto camera = player->GetComponent<CameraController>();
        Vector2 rotateAngles = camera->GetRotation();

        self->LocalTransform.SetRotation(Vector3(-rotateAngles.y, rotateAngles.x, 0.0f));
        self->LocalTransform.SetPosition(player->LocalTransform.GetPosition() + self->LocalTransform.GetRotationQuaternion() * GunRelativePosition);
    }
    
    inline static float timeSinceShoot = 0.0f;
    constexpr static float ShootInterval = 0.1f;
    constexpr static float BulletSpeed = 15.0f;
    constexpr static float BulletSize = 0.1f;
    constexpr static float GunMaxEmission = 0.5;
    constexpr static float GunEmmisionIncrease = 0.5f;
    constexpr static float GunEmmisionDecrease = 1.5f;

    void Shoot(MxObject::Handle self, MxObject::Handle player, MxObject::Handle bulletFactory)
    {
        auto camera = player->GetComponent<CameraController>();
        Vector3 viewDirection = camera->GetDirection();

        auto bullet = Instanciate(bulletFactory);

        Vector3 bulletOffset = viewDirection * 0.6f + self->LocalTransform.GetRotationQuaternion() * GunRelativePosition;
        bullet->LocalTransform.SetPosition(player->LocalTransform.GetPosition() + bulletOffset);
        bullet->LocalTransform.SetScale(BulletSize);

        bullet->AddComponent<Script>("PlazmaGunBullet");
        bullet->GetComponent<RigidBody>()->SetLinearVelocity(BulletSpeed * viewDirection);
    }

public:
    virtual void OnCreate(MxObject::Handle self) override
    {
        
    }

    virtual void OnReload(MxObject::Handle self) override
    {
        
    }

    virtual void OnUpdate(MxObject::Handle self) override
    {
        auto player = MxObject::GetByName("Player");
        auto bulletFactory = MxObject::GetByName("Bullet Factory");

        if (!player.IsValid() || !player->HasComponent<CameraController>())
            return;

        this->FollowPlayer(self, player);

        if (!bulletFactory.IsValid())
            return;

        timeSinceShoot += Time::Delta();
        if (Input::IsMouseHeld(MouseButton::LEFT) && timeSinceShoot > ShootInterval)
        {
            timeSinceShoot = 0.0f;
            this->Shoot(self, player, bulletFactory);
        }

        auto gunMaterial = self->GetComponent<MeshRenderer>()->GetMaterial();
        if (Input::IsMouseHeld(MouseButton::LEFT))
            gunMaterial->Emission = Min((gunMaterial->Emission + GunEmmisionIncrease * Time::Delta()), GunMaxEmission);
        else
            gunMaterial->Emission = Max((gunMaterial->Emission - GunEmmisionDecrease * Time::Delta()), 0.0);
    }
};

MXENGINE_RUNTIME_EDITOR(PlazmaGun);
