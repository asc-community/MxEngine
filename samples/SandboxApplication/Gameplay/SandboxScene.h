#pragma once

#include <MxEngine.h>
#include "UserObjects.h"

using namespace MxEngine;

class SandboxScene
{
public:
    void OnCreate()
    {
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

		// auto object = MxObject::Create();
		// object->AddComponent<MeshSource>(AssetManager::LoadMesh("D:/repos/glTF-Sample-Models/2.0/Sponza/glTF/Sponza.gltf"));

		auto notifier = MxObject::Create();
		notifier->Name = "Notifier";
		notifier->AddComponent<Behaviour>([](MxObject&, float dt)
		{
			MXLOG_INFO("Timer", "I am called!");
		})->Schedule(Timer::UPDATE_EACH_DELTA, 1000000000.0f);
		
		auto dirLight = MxObject::Create();
		InitDirLight(*dirLight);

		auto pointLight = MxObject::Create();
		InitPointLight(*pointLight);
		
		auto spotLight = MxObject::Create();
		InitSpotLight(*spotLight);

		auto camera = MxObject::Create();
		InitCamera(*camera);

		auto sound = MxObject::Create();
		InitSound(*sound);

		RenderManager::SetViewport(camera->GetComponent<CameraController>());
    }

	void OnUpdate()
	{
		
	}
};