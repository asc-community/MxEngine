#pragma once

#include "Vendors/bullet3/btBulletDynamicsCommon.h"
#include "Utilities/Math/Math.h"
#include "Core/Components/Transform.h"

namespace MxEngine
{
    inline btVector3& ToBulletVector3(Vector3& v) { return *reinterpret_cast<btVector3*>(&v); }
    inline Vector3& FromBulletVector3(btVector3& v) { return *reinterpret_cast<Vector3*>(&v); }
    inline btVector3 ToBulletVector3(const Vector3& v) { return btVector3(v.x, v.y, v.z); }
    inline Vector3 FromBulletVector3(const btVector3& v) { return MakeVector3(v.x(), v.y(), v.z()); }

    inline void FromBulletTransform(Transform& to, const btTransform& from)
    {
        auto rot = from.getRotation();
        to.SetPosition(FromBulletVector3(from.getOrigin()));
        to.SetRotation(*reinterpret_cast<Quaternion*>(&rot));
    }
}