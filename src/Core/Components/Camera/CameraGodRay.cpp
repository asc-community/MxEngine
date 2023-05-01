#include "CameraGodRay.h"
#include "Utilities/Math/Math.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    float CameraGodRay::GetGodRayMaxSteps()const
    {
        return this->godRayMaxSteps;
    }

    float CameraGodRay::GetGodRayStepIncrement()const
    {
        return this->godRayStepIncrement;
    }

    float CameraGodRay::GetGodRaySampleStep()const
    {
        return this->godRaySampleStep;
    }

    float CameraGodRay::GetGodRayAsymmetry()const
    {
        return this->godRayAsymmetry;
    }

    void CameraGodRay::SetGodRayMaxSteps(float num)
    {
        this->godRayMaxSteps = num;
    }

    void CameraGodRay::SetGodRayStepIncrement(float value)
    {
        this->godRayStepIncrement = value;
    }

    void CameraGodRay::SetGodRaySampleStep(float value)
    {
        this->godRaySampleStep = value;
    }

    void CameraGodRay::SetGodRayAsymmetry(float value)
    {
        this->godRayAsymmetry = value;
    }

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::class_<CameraGodRay>("CameraGodRay")
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::CLONE_COPY | MetaInfo::CLONE_INSTANCE)
            )
            .constructor<>()
            .property("god ray max steps", &CameraGodRay::GetGodRayMaxSteps, &CameraGodRay::SetGodRayMaxSteps)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 100.0f, 1000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 1.0f)
            )
            .property("god ray sample step", &CameraGodRay::GetGodRaySampleStep, &CameraGodRay::SetGodRaySampleStep)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.01f, 0.5f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.001f)
            )
            .property("god ray step increment", &CameraGodRay::GetGodRayStepIncrement, &CameraGodRay::SetGodRayStepIncrement)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 1.01f, 1.1f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.001f)
            )
            .property("god ray asymmetry", &CameraGodRay::GetGodRayAsymmetry, &CameraGodRay::SetGodRayAsymmetry)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.1f, 1.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            );
    }
}