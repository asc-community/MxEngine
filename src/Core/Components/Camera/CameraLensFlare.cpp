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

    void CameraLensFlare::SetLensFlareScale(float scale)
    {
        this->lensFlareScale = scale;
    }

    void CameraLensFlare::SetLensFlareBias(float bias)
    {
        this->lensFlareBias = bias;
    }

    void CameraLensFlare::SetLensFlareNumOfGhosts(int num)
    {
        this->lensFlareNumOfGhosts = Max(num,0);
    }

    void CameraLensFlare::SetLensFlareGhostDispersal(float dispersal)
    {
        this->lensFlareGhostDispersal = dispersal;
    }

    void CameraLensFlare::SetLensFlareHaloWidth(float width)
    {
        this->lensFalreHaloWidth = width;
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
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.001f, 1.f }),
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