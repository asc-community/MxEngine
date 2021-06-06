#include <MxEngine.h>

using namespace MxEngine;

class PlazmaGunBullet : public MxEngine::Scriptable
{
    constexpr static float BulletMass = 0.01f;
    constexpr static Vector3 BulletColor = { 0.0f, 0.6f, 1.0f };

public:
    static void CreateExplosion(const Vector3& position)
    {
        auto explosionBase = MxObject::GetByName("PlazmaGunBullet Explosion Base");
        if (!explosionBase.IsValid()) return;
        auto explosionMaterial = explosionBase->GetComponent<MeshRenderer>()->GetMaterial();
        auto explosionParticles = explosionBase->GetComponent<ParticleSystem>();
        
        auto explosion = MxObject::Create();
        explosion->LocalTransform.SetPosition(position);
        explosion->LocalTransform.SetScale(explosionBase->LocalTransform.GetScale());
        explosion->Name = "PlazmaGunBullet Explosion";
        explosion->AddComponent<MeshRenderer>(explosionMaterial);

        auto ps = explosion->AddComponent<ParticleSystem>();
        ps->SetMaxParticleCount(explosionParticles->GetMaxParticleCount());
        ps->SetParticleLifetime(explosionParticles->GetParticleLifetime());
        ps->SetMaxParticleSize(explosionParticles->GetMaxParticleSize());
        ps->SetMinParticleSize(explosionParticles->GetMinParticleSize());
        ps->SetParticleSpeed(explosionParticles->GetParticleSpeed());

        Timer::CallAfterDelta([explosion]() { MxObject::Destroy(explosion); }, ps->GetParticleLifetime());
    }

    virtual void OnCreate(MxEngine::MxObject& self) override
    {
        self.Name = "PlazmaGunBullet";
        auto collider = self.AddComponent<SphereCollider>();
        auto body = self.AddComponent<RigidBody>();
        auto instance = self.GetComponent<Instance>();

        instance->SetColor(BulletColor);

        body->MakeDynamic();
        body->SetMass(5.0f);
        body->SetOnCollisionEnterCallback(
            [](MxObject& self, MxObject& other)
            {
                if (other.Name != "Player" && other.Name != "PlazmaGunBullet")
                {
                    auto selfBody = self.GetComponent<RigidBody>();
                    auto otherBody = other.GetComponent<RigidBody>();
                    float massRatio = selfBody->GetMass() / Max(otherBody->GetMass(), 0.01f);
                    otherBody->SetLinearVelocity(selfBody->GetLinearVelocity() * massRatio);
                    PlazmaGunBullet::CreateExplosion(self.LocalTransform.GetPosition());
                    MxObject::Destroy(self);
                }
            });

        Timer::CallAfterDelta([o = MxObject::GetHandle(self)]() { MxObject::Destroy(o); }, 10.0f);
    }

    virtual void OnReload(MxEngine::MxObject& self) override
    {
        
    }

    virtual void OnUpdate(MxEngine::MxObject& self) override
    {

    }
};

MXENGINE_RUNTIME_EDITOR(PlazmaGunBullet);
