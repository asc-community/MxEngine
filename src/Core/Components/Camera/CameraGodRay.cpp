#include "CameraGodRay.h"
#include "Utilities/Math/Math.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    float GodRayEffect::GetGodRayMaxSteps()const
    {
        return this->godRayMaxSteps;
    }

    float GodRayEffect::GetGodRayStepIncrement()const
    {
        return this->godRayStepIncrement;
    }

    float GodRayEffect::GetGodRaySampleStep()const
    {
        return this->godRaySampleStep;
    }
    void GodRayEffect::SetGodRayMaxSteps(float num)
    {
        this->godRayMaxSteps = num;
    }

    void GodRayEffect::SetGodRayStepIncrement(float value)
    {
        this->godRayStepIncrement = value;
    }

    void GodRayEffect::SetGodRaySampleStep(float value)
    {
        this->godRaySampleStep = value;
    }

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::class_<GodRayEffect>("GodRayEffect")
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::CLONE_COPY | MetaInfo::CLONE_INSTANCE)
            )
            .constructor<>()
            .property("god ray max steps", &GodRayEffect::GetGodRayMaxSteps, &GodRayEffect::SetGodRayMaxSteps)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 100.0f, 1000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 1.0f)
            )
            .property("god ray sample step", &GodRayEffect::GetGodRaySampleStep, &GodRayEffect::SetGodRaySampleStep)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.01f, 0.5f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.001f)
            )
            .property("god ray step increment", &GodRayEffect::GetGodRayStepIncrement, &GodRayEffect::SetGodRayStepIncrement)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 1.01f, 1.1f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.001f)
            );
    }
}