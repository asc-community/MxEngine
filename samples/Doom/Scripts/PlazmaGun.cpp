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

    void Shoot(MxObject::Handle self, MxObject::Handle player, MxObject::Handle bulletFactory)
    {
        auto& database = self->GetComponent<Script>()->Database;
        const auto BulletSize = database.Get<float>("BulletSize");
        const auto BulletSpeed = database.Get<float>("BulletSpeed");

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
        // auto& database = self->GetComponent<Script>()->Database;
        // database.Add("TimeSinceShoot", 0.0f);
        // database.Add("ShootInterval", 0.1f);
        // database.Add("BulletSpeed", 15.0f);
        // database.Add("BulletSize", 0.1f);
        // database.Add("GunMaxEmission", 0.5);
        // database.Add("GunEmmisionIncrease", 0.5f);
        // database.Add("GunEmmisionDecrease", 1.5f);
    }

    virtual void OnUpdate(MxObject::Handle self) override
    {
        auto& database = self->GetComponent<Script>()->Database;
        auto TimeSinceShoot = database.Get<float>("TimeSinceShoot");
        const auto ShootInterval = database.Get<float>("ShootInterval");
        const auto GunEmmisionIncrease = database.Get<float>("GunEmmisionIncrease");
        const auto GunEmmisionDecrease = database.Get<float>("GunEmmisionDecrease");
        const auto GunMaxEmission = database.Get<float>("GunMaxEmission");

        auto player = MxObject::GetByName("Player");
        auto bulletFactory = MxObject::GetByName("Bullet Factory");

        if (!player.IsValid() || !player->HasComponent<CameraController>())
            return;

        this->FollowPlayer(self, player);

        if (!bulletFactory.IsValid())
            return;

        TimeSinceShoot += Time::Delta();
        if (Input::IsMouseHeld(MouseButton::LEFT) && TimeSinceShoot > ShootInterval)
        {
            TimeSinceShoot = 0.0f;
            this->Shoot(self, player, bulletFactory);
        }
        self->GetComponent<Script>()->Database.Add("TimeSinceShoot", TimeSinceShoot);

        auto gunMaterial = self->GetComponent<MeshRenderer>()->GetMaterial();
        if (Input::IsMouseHeld(MouseButton::LEFT))
            gunMaterial->Emission = Min((gunMaterial->Emission + GunEmmisionIncrease * Time::Delta()), GunMaxEmission);
        else
            gunMaterial->Emission = Max((gunMaterial->Emission - GunEmmisionDecrease * Time::Delta()), 0.0);
    }
};

MXENGINE_RUNTIME_EDITOR(PlazmaGun);
