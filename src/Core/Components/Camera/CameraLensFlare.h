#pragma once

#include "Utilities/ECS/Component.h"


namespace MxEngine
{
	class CameraLensFlare 
	{
        MAKE_COMPONENT(CameraLensFlare);
    private:
        float intensity = 0.1;
        int ghostNumber = 12;
        float ghostDispersal = 0.13f;
        float haloWidth = 0.5f;

    public:
        CameraLensFlare() = default;

        float GetIntensity() const;
        int GetGhostNumber() const;
        float GetGhostDispersal() const;
        float GetHaloWidth() const;

        void SetIntensity(float);
        void SetGhostNumber(int);
        void SetGhostDispersal(float);
        void SetHaloWidth(float);
	};
}