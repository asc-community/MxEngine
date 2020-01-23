#pragma once
#include <string>

#include "Utilities/Memory/Memory.h"
#include "Core/OpenGL/Texture/Texture.h"
#include "Utilities/Math/Math.h"

namespace MomoEngine
{
	struct Material
	{
		Ref<Texture> map_Ka;
		Ref<Texture> map_Kd;
		Ref<Texture> map_Ks;
		Ref<Texture> map_Ke;
		Ref<Texture> map_d;
		Ref<Texture> map_bump;
		Ref<Texture> bump;

		float Ns = 0.0f;
		float Ni = 0.0f;
		float d = 0.0f;
		float Tr = 0.0f;
		Vector3 Tf{ 0.0f };
		Vector3 Ka{ 0.0f };
		Vector3 Kd{ 0.0f };
		Vector3 Ks{ 0.0f };
		Vector3 Ke{ 0.0f };
		int illum = 0;
	};
}