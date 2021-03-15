#pragma once

#include <btBulletDynamicsCommon.h>

#include "Utilities/Math/Math.h"
#include "Core/Components/Transform.h"

namespace MxEngine
{
    inline btVector3 ToBulletVector3(const Vector3& v) { return btVector3(v.x, v.y, v.z); }
    inline Vector3 FromBulletVector3(const btVector3& v) { return MakeVector3(v.x(), v.y(), v.z()); }

    inline void FromBulletTransform(TransformComponent& to, const btTransform& from)
    {
        auto rot = from.getRotation();
        to.SetPosition(FromBulletVector3(from.getOrigin()));
        to.SetRotation(*reinterpret_cast<Quaternion*>(&rot));
    }

    inline void ToBulletTransform(btTransform& to, const TransformComponent& from)
    {
        auto rot = from.GetRotationQuaternion();
        to.setIdentity();
        to.setOrigin(ToBulletVector3(from.GetPosition()));
        to.setRotation(*reinterpret_cast<btQuaternion*>(&rot));
    }
}