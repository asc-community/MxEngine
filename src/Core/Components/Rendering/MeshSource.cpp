#include "MeshSource.h"
#include "Core/Runtime/Reflection.h"

MXENGINE_FORCE_REFLECTION_IMPLEMENTATION(MeshSource);

namespace MxEngine
{
    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::class_<MeshSource>("MeshSource")
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::CLONE_COPY)
            )
            .constructor<>()
            .property("is drawn", &MeshSource::IsDrawn)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("is static", &MeshSource::IsStatic)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("casts shadow", &MeshSource::CastsShadow)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("ignores depth", &MeshSource::IgnoresDepth)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("mesh", &MeshSource::Mesh)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            );
    }
}