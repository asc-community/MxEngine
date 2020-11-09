#pragma once

#include <MxEngine.h>
#include "UserObjects.h"

using namespace MxEngine;

class SandboxScene
{
public:
    void OnCreate()
    {
		auto camera = MxObject::Create();
		InitCamera(*camera);
		Rendering::SetViewport(camera->GetComponent<CameraController>());

		auto cube = MxObject::Create();
		InitCube(*cube);
		
		auto sphere = MxObject::Create();
		InitSphere(*sphere);
		
		auto grid = MxObject::Create();
		InitGrid(*grid);
		
		auto surface = MxObject::Create();
		InitSurface(*surface);

        // auto arc = MxObject::Create();
		// InitArc(*arc);

		// auto& deathStar = this->AddObject("DeathStar", MakeUnique<MxObject>());
		// InitDeathStar(deathStar);

		// auto& destroyer = this->AddObject("Destroyer", MakeUnique<MxObject>());
		// InitDestroyer(destroyer);
		
		auto dirLight = MxObject::Create();
		InitDirLight(*dirLight);

		auto pointLight = MxObject::Create();
		InitPointLight(*pointLight);
		
		auto spotLight = MxObject::Create();
		InitSpotLight(*spotLight);

		auto pbr = MxObject::Create();
		InitPBRObject(*pbr);

		// auto sound = MxObject::Create();
		// InitSound(*sound);
    }

	void OnUpdate()
	{
		
	}
};