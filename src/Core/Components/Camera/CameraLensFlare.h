#pragma once

#include "Utilities/ECS/Component.h"


namespace MxEngine
{
	class CameraLensFlare 
	{
        MAKE_COMPONENT(CameraLensFlare);
    private:
        float lensFlareScale = 1.f;
        float lensFlareBias = -42;
        int lensFlareNumOfGhosts = 10;
        float lensFlareGhostDispersal = 0.13f;
        float lensFalreHaloWidth = 0.5f;
        bool lensFlareEnable = true;

    public:
        CameraLensFlare() = default;
        float GetLensFlareScale()const;
        float GetLensFlareBias()const;
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