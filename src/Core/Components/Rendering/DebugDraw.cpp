#include "DebugDraw.h"
#include "Core/Runtime/Reflection.h"

MXENGINE_FORCE_REFLECTION_IMPLEMENTATION(DebugDraw);
 
namespace MxEngine
{

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::class_<DebugDraw>("DebugDraw")
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::CLONE_COPY | MetaInfo::CLONE_INSTANCE)
            )
            .constructor<>()
            .property("bounding box color", &DebugDraw::BoundingBoxColor)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::INTERPRET_AS, InterpretAsInfo::COLOR)
            )
            .property("bounding sphere color", &DebugDraw::BoundingSphereColor)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::INTERPRET_AS, InterpretAsInfo::COLOR)
            )
            .property("light source color", &DebugDraw::LightSourceColor)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::INTERPRET_AS, InterpretAsInfo::COLOR)
            )
            .property("sound source color", &DebugDraw::SoundSourceColor)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::INTERPRET_AS, InterpretAsInfo::COLOR)
            )
            .property("frustrum color", &DebugDraw::FrustrumColor)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::INTERPRET_AS, InterpretAsInfo::COLOR)
            )
            .property("render physics collider", &DebugDraw::RenderPhysicsCollider)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("render bounding box", &DebugDraw::RenderBoundingBox)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("render bounding sphere", &DebugDraw::RenderBoundingSphere)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("render lighting bounds", &DebugDraw::RenderLightingBounds)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("render sound bounds", &DebugDraw::RenderSoundBounds)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("render frustrum bounds", &DebugDraw::RenderFrustrumBounds)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            );
    }
}