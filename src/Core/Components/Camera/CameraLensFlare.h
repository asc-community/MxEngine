#pragma once

#include "Utilities/ECS/Component.h"


namespace MxEngine
{
	class CameraLensFlare 
	{
        MAKE_COMPONENT(CameraLensFlare);
    private:
        float lensFlareScale = 1.0f;
        float lensFlareBias = -42.0f;
        int lensFlareNumOfGhosts = 12;
        float lensFlareGhostDispersal = 0.13f;
        float lensFalreHaloWidth = 0.5f;

    public:
        CameraLensFlare() = default;
        int GetLensFlareNumOfGhosts()const;
        float GetLensFlareGhostDispersal()const;
        float GetLensFlareHaloWidth()const;

        void SetLensFlareScale(float);
        void SetLensFlareBias(float);
        void SetLensFlareNumOfGhosts(int);
        void SetLensFlareGhostDispersal(float);
        void SetLensFlareHaloWidth(float);
	};
}