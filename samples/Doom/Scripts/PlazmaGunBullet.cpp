#include <MxEngine.h>

using namespace MxEngine;

class PlazmaGunBullet : public MxEngine::Scriptable
{
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

    virtual void OnCreate(MxObject::Handle self) override
    {
        self->Name = "PlazmaGunBullet";
        auto collider = self->AddComponent<SphereCollider>();
        auto instance = self->GetComponent<Instance>();
        auto body = self->AddComponent<RigidBody>();

        instance->SetColor(Vector3{ 0.0f, 0.6f, 1.0f });

        body->SetMass(0.1f);
        body->MakeTrigger();
        body->SetCollisionFilter(
            body->GetCollisionMask() | CollisionMask::DYNAMIC, 
            body->GetCollisionGroup() | CollisionGroup::ALL
        );
        body->SetGravity({ 0.0f, 0.0f, 0.0f });
        body->SetOnCollisionEnterCallback(
            [](MxObject::Handle self, MxObject::Handle other)
            {
                if (other->Name != "Player" && other->Name != "PlazmaGunBullet")
                {
                    self->GetComponent<RigidBody>()->SyncObjectState();
                    PlazmaGunBullet::CreateExplosion(self->LocalTransform.GetPosition());
                    MxObject::Destroy(self);
                }
            });

        Timer::CallAfterDelta([self]() { MxObject::Destroy(self); }, 10.0f);
    }

    virtual void OnReload(MxObject::Handle self) override
    {

    }

    virtual void OnUpdate(MxObject::Handle self) override
    {

    }
};

MXENGINE_RUNTIME_EDITOR(PlazmaGunBullet);
