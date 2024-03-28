#include "CameraLensFlare.h"
#include "Utilities/Math/Math.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    float CameraLensFlare::GetIntensity() const
    {
        return this->intensity;
    }

    int CameraLensFlare::GetGhostNumber() const
    {
        return this->ghostNumber;
    }

    float CameraLensFlare::GetGhostDispersal()const
    {
        return this->ghostDispersal;
    }

    float CameraLensFlare::GetHaloWidth()const
    {
        return this->haloWidth;
    }

    void CameraLensFlare::SetIntensity(float scale)
    {
        this->intensity = Max(scale, 0.0);
    }

    void CameraLensFlare::SetGhostNumber(int num)
    {
        this->ghostNumber = Max(num,0);
    }

    void CameraLensFlare::SetGhostDispersal(float dispersal)
    {
        this->ghostDispersal = dispersal;
    }

    void CameraLensFlare::SetHaloWidth(float width)
    {
        this->haloWidth = width;
    }

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::class_<CameraLensFlare>("CameraLensFlare")
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::CLONE_COPY | MetaInfo::CLONE_INSTANCE)
            )
            .constructor<>()
            .property("intensity", &CameraLensFlare::GetIntensity, &CameraLensFlare::SetIntensity)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 1.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("ghost number", &CameraLensFlare::GetGhostNumber, &CameraLensFlare::SetGhostNumber)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 100.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("ghost dispersal", &CameraLensFlare::GetGhostDispersal, &CameraLensFlare::SetGhostDispersal)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.001f, 1.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("halo width", &CameraLensFlare::GetHaloWidth, &CameraLensFlare::SetHaloWidth)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 100.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            );
    }
}