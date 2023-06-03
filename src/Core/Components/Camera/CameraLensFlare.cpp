#include "CameraLensFlare.h"
#include "Utilities/Math/Math.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    float CameraLensFlare::GetLensFlareScale()const
    {
        return this->lensFlareScale;
    }

    float CameraLensFlare::GetLensFlareBias()const
    {
        return this->lensFlareBias;
    }

    int CameraLensFlare::GetLensFlareNumOfGhosts()const
    {
        return this->lensFlareNumOfGhosts;
    }

    float CameraLensFlare::GetLensFlareGhostDispersal()const
    {
        return this->lensFlareGhostDispersal;
    }

    float CameraLensFlare::GetLensFlareHaloWidth()const
    {
        return this->lensFalreHaloWidth;
    }

    void CameraLensFlare::SetLensFlareScale(float v)
    {
        this->lensFlareScale = v;
    }

    void CameraLensFlare::SetLensFlareBias(float v)
    {
        this->lensFlareBias = v;
    }

    void CameraLensFlare::SetLensFlareNumOfGhosts(int v)
    {
        this->lensFlareNumOfGhosts = v;
    }

    void CameraLensFlare::SetLensFlareGhostDispersal(float v)
    {
        this->lensFlareGhostDispersal = v;
    }

    void CameraLensFlare::SetLensFlareHaloWidth(float v)
    {
        this->lensFalreHaloWidth = v;
    }

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::class_<CameraLensFlare>("CameraLensFlare")
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::CLONE_COPY | MetaInfo::CLONE_INSTANCE)
            )
            .constructor<>()
            .property("lens flare scale", &CameraLensFlare::GetLensFlareScale, &CameraLensFlare::SetLensFlareScale)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 100.f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("lens flare bias", &CameraLensFlare::GetLensFlareBias, &CameraLensFlare::SetLensFlareBias)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { -200.0f, 0.f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("lens flare num of ghosts", &CameraLensFlare::GetLensFlareNumOfGhosts, &CameraLensFlare::SetLensFlareNumOfGhosts)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 100.f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("lens flare ghost dispersal", &CameraLensFlare::GetLensFlareGhostDispersal, &CameraLensFlare::SetLensFlareGhostDispersal)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 100.f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("lens falre halo width", &CameraLensFlare::GetLensFlareHaloWidth, &CameraLensFlare::SetLensFlareHaloWidth)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 100.f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            );
    }
}